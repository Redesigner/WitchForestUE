// Copyright (c) 2024 Stephen Melnick


#include "WitchForestAbility/Abilities/WitchForestGameplayAbility.h"

#include "AIController.h"


void UWitchForestGameplayAbility::ActivateAbilityFailed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, int16 PredictionKey)
{
}

void UWitchForestGameplayAbility::ActivateAbilitySucceed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FPredictionKey& PredictionKey)
{
}

UBlackboardComponent* UWitchForestGameplayAbility::GetOwnerBlackboard(bool& bBlackboardFound)
{
	if (!CurrentActorInfo->AvatarActor.IsValid())
	{
		bBlackboardFound = false;
		return nullptr;
	}

	APawn* OwningPawn = Cast<APawn>(CurrentActorInfo->AvatarActor);
	if (!OwningPawn)
	{
		bBlackboardFound = false;
		return nullptr;
	}

	AAIController* AIController = Cast<AAIController>(OwningPawn->GetController());
	if (!AIController)
	{
		bBlackboardFound = false;
		return nullptr;
	}

	if (AIController->GetBlackboardComponent())
	{
		bBlackboardFound = true;
		return AIController->GetBlackboardComponent();
	}

	bBlackboardFound = false;
	return nullptr;
}
