// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h" 

#include "HealthViewModel.generated.h"

class UWitchForestASC;
class UBaseAttributeSet;
struct FOnAttributeChangeData;

UCLASS()
class WITCHFORESTUI_API UHealthViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, BlueprintReadOnly,FieldNotify, meta = (AllowPrivateAccess = true))
	float CurrentHealth;

	UPROPERTY(BlueprintReadOnly, FieldNotify, meta = (AllowPrivateAccess = true))
	float MaxHealth;

public:
	UFUNCTION(BlueprintCallable)
	void BindAttributeSet(UWitchForestASC* AbilitySystemComponent, UBaseAttributeSet* AttributeSet);

	void HealthChanged(const FOnAttributeChangeData& Data);
};