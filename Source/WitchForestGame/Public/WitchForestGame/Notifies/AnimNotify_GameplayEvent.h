// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"

#include "GameplayTagContainer.h"

#include "AnimNotify_GameplayEvent.generated.h"

/**
 * 
 */
UCLASS()
class WITCHFORESTGAME_API UAnimNotify_GameplayEvent : public UAnimNotify
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Event, meta = (AllowPrivateAccess = true, Categories = GameplayEvent))
	FGameplayTag EventTag;
	
	UAnimNotify_GameplayEvent();

	void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
