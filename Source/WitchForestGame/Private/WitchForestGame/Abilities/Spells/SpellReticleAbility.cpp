// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Abilities/Spells/SpellReticleAbility.h"

#include "Abilities/GameplayAbilityTargetActor.h"
#include "Logging/StructuredLog.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "WitchForestGame.h"
#include "WitchForestGame/Abilities/Spells/SpellProjectile.h"
#include "WitchForestGame/Abilities/Tasks/WitchAbilityTask_WaitTargetData.h"
#include "WitchForestGame/Effects/EffectApplicationComponent.h"
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

	if (APawn* Pawn = Cast<APawn>(ActorInfo->AvatarActor))
	{
		if (UCharacterMovementComponent* CharacterMovementComponent = Cast<UCharacterMovementComponent>(Pawn->GetMovementComponent()) )
		{
			CharacterMovementComponent->bOrientRotationToMovement = false;
		}
	}
}


void USpellReticleAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// CommitAbilityCooldown(Handle, ActorInfo, ActivationInfo, true);

	if (APawn* Pawn = Cast<APawn>(ActorInfo->AvatarActor))
	{
		if (UCharacterMovementComponent* CharacterMovementComponent = Cast<UCharacterMovementComponent>(Pawn->GetMovementComponent()))
		{
			CharacterMovementComponent->bOrientRotationToMovement = true;
		}
	}

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
	CommitExecute(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);

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
	AActor* Projectile = GetWorld()->SpawnActorDeferred<AActor>(ProjectileClass.Get(), ProjectileTransform);

	if (UEffectApplicationComponent* EffectApplier = Projectile->GetComponentByClass<UEffectApplicationComponent>())
	{
		EffectApplier->SetEffectHandles(CurrentActorInfo->AvatarActor.Get(), EffectContainer.MakeEffectSpecs(this));
	}
	Projectile->FinishSpawning(ProjectileTransform);
}
