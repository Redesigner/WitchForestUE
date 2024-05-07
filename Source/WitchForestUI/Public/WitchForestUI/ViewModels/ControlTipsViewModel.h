// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"

#include "GameplayTagContainer.h"

#include "ControlTipsViewModel.generated.h"


class UWitchForestASC;
class AWitchPlayerController;
class UWitchForestInputHints;

UCLASS(BlueprintType)
class WITCHFORESTUI_API UControlTipUiSet : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	FText ControlTipText;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UTexture2D> DisplayIcon;
};

UCLASS()
class WITCHFORESTUI_API UControlTipsViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()


	UPROPERTY(BlueprintReadOnly, FieldNotify, meta = (AllowPrivateAccess = true))
	TArray<TObjectPtr<UControlTipUiSet>> ControlHints;


	TArray<FGameplayTag> WatchedInputTags;

	TWeakObjectPtr<UWitchForestASC> OwningASC;

	TObjectPtr<UWitchForestInputHints> InputHints;


	void PotentialAbilityActivationChanged();

	void InventoryChanged(FGameplayTag Item, uint8 SlotIndex);
	
public:
	UFUNCTION(BlueprintCallable)
	void BindAbilitySystem(UWitchForestASC* AbilitySystem, AWitchPlayerController* PlayerController, TArray<FGameplayTag> WatchedTags);

	UFUNCTION(BlueprintCallable)
	void SetInputHints(UWitchForestInputHints* InputHintsIn);
};
