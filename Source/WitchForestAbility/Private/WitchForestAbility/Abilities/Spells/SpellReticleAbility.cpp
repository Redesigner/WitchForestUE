// Copyright (c) 2024 Stephen Melnick


#include "WitchForestAbility/Abilities/Spells/SpellReticleAbility.h"

#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "Logging/StructuredLog.h"
#include "AbilitySystemComponent.h"

#include "WitchForestAbility.h"
#include "WitchForestAbility/Abilities/Spells/SpellProjectile.h"

void USpellReticleAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!TargetingClass)
	{
		UE_LOGFMT(LogWitchForestAbility, Error, "SpellReticleAbility '{SelfName}' failed to activate. ReticleClass was invalid.", GetName());
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!ActorInfo->AvatarActor.IsValid())
	{
		UE_LOGFMT(LogWitchForestAbility, Error, "SpellReticleAbility '{SelfName}' failed to activate. The activating ASC does not have an avatar.", GetName());
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilityTask_WaitTargetData* WaitTargetTask = UAbilityTask_WaitTargetData::WaitTargetData(this, "WaitTargetData", EGameplayTargetingConfirmation::Custom, TargetingClass);
	AGameplayAbilityTargetActor* SpawnedTarget;
	WaitTargetTask->BeginSpawningActor(this, TargetingClass, SpawnedTarget);

	if (!SpawnedTarget)
	{
		UE_LOGFMT(LogWitchForestAbility, Error, "Reticle class '{ClassName}' failed to spawn.", TargetingClass->GetName());
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	WaitTargetTask->FinishSpawningActor(this, SpawnedTarget);
	WaitTargetTask->ReadyForActivation();

	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	if (!ASC)
	{
		UE_LOGFMT(LogWitchForestAbility, Error, "SpellReticleAbility '{AbilityName}' failed to activate. The activating ASC was invalid.", GetName());
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	OnTargetDataReadyCallbackDelegateHandle = ASC->AbilityTargetDataSetDelegate(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey()).AddUObject(this, &ThisClass::OnTargetDataReadyCallback);
}


void USpellReticleAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (!ActorInfo->AbilitySystemComponent.IsValid())
	{
		UE_LOGFMT(LogWitchForestAbility, Error, "SpellReticleAbility '{AbilityName}' failed to consume target data on EndAbility. The activating ASC was invalid.", GetName());
		Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
		return;
	}

	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	ASC->AbilityTargetDataSetDelegate(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey()).Remove(OnTargetDataReadyCallbackDelegateHandle);
	ASC->ConsumeClientReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey());

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}


void USpellReticleAbility::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	ConfirmTaskByInstanceName("WaitTargetData", true);
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}


void USpellReticleAbility::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData, FGameplayTag ApplicationTag)
{
	UAbilitySystemComponent* MyAbilityComponent = CurrentActorInfo->AbilitySystemComponent.Get();
	check(MyAbilityComponent);

	if (const FGameplayAbilitySpec* AbilitySpec = MyAbilityComponent->FindAbilitySpecFromHandle(CurrentSpecHandle))
	{
		FScopedPredictionWindow	ScopedPrediction(MyAbilityComponent);

		// Take ownership of the target data to make sure no callbacks into game code invalidate it out from under us
		FGameplayAbilityTargetDataHandle LocalTargetDataHandle(MoveTemp(const_cast<FGameplayAbilityTargetDataHandle&>(InData)));

		const bool bShouldNotifyServer = CurrentActorInfo->IsLocallyControlled() && !CurrentActorInfo->IsNetAuthority();
		if (bShouldNotifyServer)
		{
			MyAbilityComponent->CallServerSetReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey(), LocalTargetDataHandle, ApplicationTag, MyAbilityComponent->ScopedPredictionKey);
			
			for (int i = 0; i < LocalTargetDataHandle.Num(); ++i)
			{
				FGameplayAbilityTargetData* TargetData = LocalTargetDataHandle.Get(i);
				FVector ProjectileSpawnLocation = TargetData->GetEndPoint();
				ProjectileSpawnLocation.Z += ProjectileSpawnHeight;
				SpawnProjectile(ProjectileSpawnLocation);
			}
		}
	}
}


void USpellReticleAbility::SpawnProjectile(FVector Location)
{
	if (!ProjectileClass)
	{
		UE_LOGFMT(LogWitchForestAbility, Error, "SpellReticleAbility '{AbilityName}' failed to spawn projectile. The projectile class was invalid.", GetName());
		return;
	}

	FRotator SpawnRotation;
	GetWorld()->SpawnActor<ASpellProjectile>(ProjectileClass, Location, SpawnRotation);
}
