// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"

#include "WitchForestAbility/Effects/GameplayEffectUIDataComponent.h"

#include "GameplayTagContainer.h"
#include "GameplayEffect.h"

#include "EffectDisplayViewModel.generated.h"


class UAbilitySystemComponent;

UCLASS(BlueprintType)
class WITCHFORESTUI_API UGameplayEffectUIDataWrapper : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	FWitchForestGameplayEffectUIData Data;
};

UCLASS()
class WITCHFORESTUI_API UEffectDisplayViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void BindAbilitySystemComponent(UAbilitySystemComponent* AbilitySystemComponent);

private:
	void GameplayEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& GameplayEffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle);


	UPROPERTY(BlueprintReadOnly, FieldNotify, meta = (AllowPrivateAccess = true))
	TArray<TObjectPtr<UGameplayEffectUIDataWrapper>> CurrentEffects;

	TWeakObjectPtr<UAbilitySystemComponent> OwningASC;
};
