// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"

#include "AbilitySystemComponent.h"

#include "BaseAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class WITCHFORESTABILITY_API UBaseAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

	// This probably isn't ideal, but it works fine for multiplicative modififers
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Movement, meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MovementSpeedModifier;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Health, ReplicatedUsing = OnRep_Health, meta = (/* HideFromModifiers, */ AllowPrivateAccess = true))
	FGameplayAttributeData Health;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Health, ReplicatedUsing = OnRep_MaxHealth, meta = (/* HideFromModifiers, */ AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;

	UPROPERTY(BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Damage;

public:
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, MovementSpeedModifier)
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Health)
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, MaxHealth)
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Damage)

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnDeath, const FGameplayEffectSpec)
	FOnDeath OnDeath;

private:
	float MaxHealthBeforeAttributeChange;
	float HealthBeforeAttributeChange;


	UBaseAttributeSet();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;

	void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

	void BroadcastDamageEventPerception(const FGameplayEffectModCallbackData& Data);


	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);
};
