// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"

#include "WitchForestGame/Dynamic/Bestiary/BestiaryData.h"

#include "BestiaryViewModel.generated.h"

class AWitchForestGameState;

UCLASS(BlueprintType)
class WITCHFORESTUI_API UBestiaryUIData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	FBestiaryEntry BestiaryEntry;
};

UCLASS()
class WITCHFORESTUI_API UBestiaryViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, FieldNotify, meta = (AllowPrivateAccess = true))
	TArray<TObjectPtr<UBestiaryUIData>> KnownBestiaryEntries;

	TWeakObjectPtr<AWitchForestGameState> OwningGameState;

	UFUNCTION(BlueprintCallable)
	void BindGameState(AWitchForestGameState* GameState);

	void CreatureDiscovered();
};
