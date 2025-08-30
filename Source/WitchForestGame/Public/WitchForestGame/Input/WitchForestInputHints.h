// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "GameplayTagContainer.h"

#include "WitchForestInputHints.generated.h"


USTRUCT(BlueprintType)
struct FWitchForestInputHint
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;

	UPROPERTY(EditDefaultsOnly)
	FName GamepadButtonName;

	UPROPERTY(EditDefaultsOnly)
	FName KeyboardButtonName;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> GamepadButtonIcon;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> KeyboardButtonIcon;
};

UCLASS()
class WITCHFORESTGAME_API UWitchForestInputHints : public UDataAsset
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty = "InputTag", AllowPrivateAccess = true))
	TArray<FWitchForestInputHint> Hints;

public:
	FName FindGamepadButtonNameForInputTag(const FGameplayTag& InputTag) const;

	FName FindKeyboardButtonNameForInputTag(const FGameplayTag& InputTag) const;

	UTexture2D* FindGamepadButtonIconForInputTag(const FGameplayTag& InputTag) const;

	UTexture2D* FindKeyboardButtonIconForInputTag(const FGameplayTag& InputTag) const;
};
