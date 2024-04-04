// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"

#include "GameplayTagContainer.h"

#include "ControlTipsViewModel.generated.h"


class UWitchForestASC;
class AWitchPlayerController;

UCLASS()
class WITCHFORESTUI_API UControlTipsViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, FieldNotify, meta = (AllowPrivateAccess = true))
	FText ControlHints;


	TArray<FGameplayTag> WatchedInputTags;

	TWeakObjectPtr<UWitchForestASC> OwningASC;


	void PotentialAbilityActivationChanged();
	
public:
	UFUNCTION(BlueprintCallable)
	void BindAbilitySystem(UWitchForestASC* AbilitySystem, AWitchPlayerController* PlayerController, TArray<FGameplayTag> WatchedTags);
};
