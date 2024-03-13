// Copyright (c) 2024 Stephen Melnick


#include "WitchForestAbility/Abilities/DeathAbility.h"

#include "WitchForestGame/WitchForestGameplayTags.h"
#include "WitchForestGame/Character/Components/ItemHandleComponent.h"
#include "WitchForestGame/Character/Witch/SoulSprite.h"
#include "WitchForestGame/Inventory/InventoryComponent.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/PlayerState.h"

UDeathAbility::UDeathAbility()
{
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		// Add the ability trigger tag as default to the CDO.
		FAbilityTriggerData TriggerData;
		TriggerData.TriggerTag = WitchForestGameplayTags::GameplayEvent_Death;
		TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
		AbilityTriggers.Add(TriggerData);
	}
}


void UDeathAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!ActorInfo->AvatarActor.IsValid())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	APawn* AvatarPawn = Cast<APawn>(ActorInfo->AvatarActor);
	if (!AvatarPawn)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AController* AvatarController = AvatarPawn->GetController();
	AvatarPawn->DetachFromControllerPendingDestroy();
	AvatarPawn->SetLifeSpan(0.1f);

	if (SpawnedSpriteClass && GetWorld() && AvatarController)
	{
		ASoulSprite* SoulSprite = GetWorld()->SpawnActor<ASoulSprite>(SpawnedSpriteClass, AvatarPawn->GetActorLocation(), FRotator());

		if (APlayerController* PlayerController = Cast<APlayerController>(AvatarController))
		{
			PlayerController->SetViewTarget(SoulSprite);
			SoulSprite->SetOwningASC(ActorInfo->AbilitySystemComponent.Get());
			ActorInfo->AbilitySystemComponent->SetAvatarActor(SoulSprite);
		}

		/*
		FTimerHandle RespawnTimer;
		FTimerDelegate RespawnDelegate;
		RespawnDelegate.BindLambda([AvatarController]() {
			if (APlayerController* PlayerController = Cast<APlayerController>(AvatarController))
			{
				if (APawn* OldPawn = PlayerController->GetPawn())
				{
					OldPawn->DetachFromControllerPendingDestroy();
					OldPawn->SetLifeSpan(0.1f);
				}
				PlayerController->ServerRestartPlayer_Implementation();
			}
			});
		GetWorld()->GetTimerManager().SetTimer(RespawnTimer, RespawnDelegate, 5.0f, false);
		*/
	}

	if (UItemHandleComponent* ItemHandle = AvatarPawn->GetComponentByClass<UItemHandleComponent>())
	{
		ItemHandle->ConsumeItem();
	}

	if (UInventoryComponent* InventoryComponent = ActorInfo->OwnerActor->GetComponentByClass<UInventoryComponent>())
	{
		InventoryComponent->DropItems();
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}