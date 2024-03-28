// Copyright (c) 2024 Stephen Melnick


#include "WitchForestAbility/Abilities/Spells/SpellReticleAbility.h"

#include "Abilities/GameplayAbilityTargetActor.h"
#include "Logging/StructuredLog.h"
#include "AbilitySystemComponent.h"

#include "WitchForestAbility.h"
#include "WitchForestAbility/Abilities/Spells/SpellProjectile.h"
#include "WitchForestAbility/Abilities/Tasks/WitchAbilityTask_WaitTargetData.h"
#include "WitchForestGame/WitchForestGameplayTags.h"

void USpellReticleAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!TargetingClass)
	{
		UE_LOGFMT(LogWitchForestAbility, Error, "SpellReticleAbility '{SelfName}' failed to activate. ReticleClass was invalid.", GetName());
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// UAbilityTask_WaitTargetData* WaitTargetTask = UWitchAbilityTask_WaitTargetData::Witch_WaitTargetData(this, "WaitTargetData", EGameplayTargetingConfirmation::Custom, TargetingClass);
	UAbilityTask_WaitTargetData* WaitTargetTask = UAbilityTask_WaitTargetData::WaitTargetData(this, "WaitTargetData", EGameplayTargetingConfirmation::Custom, TargetingClass);
	WaitTargetTask->ValidData.AddUniqueDynamic(this, &ThisClass::OnTargetDataReadyCallback);
	WaitTargetTask->ReadyForActivation();
	AGameplayAbilityTargetActor* TargetActor;
	WaitTargetTask->BeginSpawningActor(this, TargetingClass, TargetActor);
	WaitTargetTask->FinishSpawningActor(this, TargetActor);
}


void USpellReticleAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}


void USpellReticleAbility::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	ConfirmTaskByInstanceName("WaitTargetData", true);
}


void USpellReticleAbility::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData)
{
	UAbilitySystemComponent* MyAbilityComponent = CurrentActorInfo->AbilitySystemComponent.Get();
	check(MyAbilityComponent);

	const FGameplayAbilitySpec* AbilitySpec = MyAbilityComponent->FindAbilitySpecFromHandle(CurrentSpecHandle);
	if (!AbilitySpec)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	FScopedPredictionWindow	ScopedPrediction(MyAbilityComponent);

	// Take ownership of the target data to make sure no callbacks into game code invalidate it out from under us
	FGameplayAbilityTargetDataHandle LocalTargetDataHandle(MoveTemp(const_cast<FGameplayAbilityTargetDataHandle&>(InData)));

	const bool bShouldNotifyServer = CurrentActorInfo->IsLocallyControlled() && !CurrentActorInfo->IsNetAuthority();
	if (bShouldNotifyServer)
	{
		MyAbilityComponent->CallServerSetReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey(), LocalTargetDataHandle, FGameplayTag(), MyAbilityComponent->ScopedPredictionKey);
	}

	if (HasAuthority(&CurrentActivationInfo))
	{
		for (int i = 0; i < LocalTargetDataHandle.Num(); ++i)
		{
			FGameplayAbilityTargetData* TargetData = LocalTargetDataHandle.Get(i);
			FVector ProjectileSpawnLocation = TargetData->GetEndPoint();
			ProjectileSpawnLocation.Z += ProjectileSpawnHeight;

			if (TargetData->GetHitResult() && TargetData->GetHitResult()->bBlockingHit)
			{
				SpawnProjectile(ProjectileSpawnLocation);
			}
		}
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}


void USpellReticleAbility::SpawnProjectile(FVector Location)
{
	if (!ProjectileClass)
	{
		UE_LOGFMT(LogWitchForestAbility, Error, "SpellReticleAbility '{AbilityName}' failed to spawn projectile. The projectile class was invalid.", GetName());
		return;
	}

	FQuat SpawnRotation;
	if (CurrentActorInfo->AvatarActor.IsValid())
	{
		SpawnRotation = CurrentActorInfo->AvatarActor->GetActorQuat();
	}

	FTransform ProjectileTransform;
	ProjectileTransform.SetLocation(Location);
	ProjectileTransform.SetRotation(SpawnRotation);
	ASpellProjectile* Projectile = GetWorld()->SpawnActorDeferred<ASpellProjectile>(ProjectileClass.Get(), ProjectileTransform);
	TArray<FGameplayEffectSpecHandle> NewEffectSpecs;


	for (TSubclassOf<UGameplayEffect> SpellEffect : SpellEffects)
	{
		FGameplayEffectSpecHandle NewEffectSpec = MakeOutgoingGameplayEffectSpec(SpellEffect);
		if (!NewEffectSpec.IsValid())
		{
			UE_LOGFMT(LogWitchForestAbility, Error, "SpellReticleAbility '{AbilityName}' failed to create spec of GameplayEffect '{GameplayEffectName}'.", GetName(), SpellEffect->GetName());
			return;
		}

		FGameplayEffectSpec* Spec = NewEffectSpec.Data.Get();
		if (Spec)
		{
			Spec->SetSetByCallerMagnitude(WitchForestGameplayTags::SetByCaller_Damage, -DamageAmount);
		}
		
		NewEffectSpecs.Add(NewEffectSpec);
	}

	Projectile->SetEffectHandles(NewEffectSpecs);
	Projectile->SetOwningActor(CurrentActorInfo->AvatarActor.Get());
	Projectile->FinishSpawning(ProjectileTransform);
}
