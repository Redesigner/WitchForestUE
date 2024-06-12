// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "ManaViewModel.generated.h"

class UWitchForestASC;
class UWitchAttributeSet;
struct FOnAttributeChangeData;

UCLASS()
class WITCHFORESTUI_API UManaViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()
	

	UPROPERTY(BlueprintReadOnly, FieldNotify, meta = (AllowPrivateAccess = true))
	float CurrentMana;

	UPROPERTY(BlueprintReadOnly, FieldNotify, meta = (AllowPrivateAccess = true))
	float MaxMana;

public:
	UFUNCTION(BlueprintCallable)
	void BindAttributeSet(UWitchForestASC* AbilitySystemComponent, UWitchAttributeSet* AttributeSet);

	void ManaChanged(const FOnAttributeChangeData& Data);

	void MaxManaChanged(const FOnAttributeChangeData& Data);
};
