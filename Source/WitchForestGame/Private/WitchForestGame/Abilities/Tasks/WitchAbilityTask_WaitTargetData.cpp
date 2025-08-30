// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Abilities/Tasks/WitchAbilityTask_WaitTargetData.h"

#include "Abilities/GameplayAbilityTargetActor.h"
#include "AbilitySystemComponent.h"


UWitchAbilityTask_WaitTargetData* UWitchAbilityTask_WaitTargetData::Witch_WaitTargetData(UGameplayAbility* OwningAbility, FName TaskInstanceName, TEnumAsByte<EGameplayTargetingConfirmation::Type> ConfirmationType, TSubclassOf<AGameplayAbilityTargetActor> Class)
{
	UWitchAbilityTask_WaitTargetData* MyObj = NewAbilityTask<UWitchAbilityTask_WaitTargetData>(OwningAbility, TaskInstanceName);		//Register for task list here, providing a given FName as a key
	MyObj->TargetClass = Class;
	MyObj->TargetActor = nullptr;
	MyObj->ConfirmationType = ConfirmationType;
	return MyObj;
}

void UWitchAbilityTask_WaitTargetData::Activate()
{
	AGameplayAbilityTargetActor* SpawnedActor;
	BeginSpawningActor(Ability, TargetClass, SpawnedActor);
	FinishSpawningActor(Ability, SpawnedActor);
}

void UWitchAbilityTask_WaitTargetData::RegisterTargetDataCallbacks()
{
	if (!ensure(IsValid(this)))
	{
		return;
	}

	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (!ASC)
	{
		return;
	}

	check(TargetClass);
	check(Ability);

	const AGameplayAbilityTargetActor* CDO = CastChecked<AGameplayAbilityTargetActor>(TargetClass->GetDefaultObject());

	const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
	const bool bShouldProduceTargetDataOnServer = CDO->ShouldProduceTargetDataOnServer;

	// If not locally controlled (server for remote client), see if TargetData was already sent
	// else register callback for when it does get here.
	if (!bIsLocallyControlled)
	{
		// Register with the TargetData callbacks if we are expecting client to send them
		if (!bShouldProduceTargetDataOnServer)
		{
			FGameplayAbilitySpecHandle	SpecHandle = GetAbilitySpecHandle();
			FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();

			//Since multifire is supported, we still need to hook up the callbacks
			ASC->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this, &UAbilityTask_WaitTargetData::OnTargetDataReplicatedCallback);
			ASC->AbilityTargetDataCancelledDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this, &UAbilityTask_WaitTargetData::OnTargetDataReplicatedCancelledCallback);

			ASC->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);

			SetWaitingOnRemotePlayerData();
		}
	}
}
