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
	
	UBaseAttributeSet();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;


public:

	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);


	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Movement)
	FGameplayAttributeData MovementSpeed;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Health, ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Health, ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, MaxHealth)

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnDeath, const FGameplayEffectSpec)
	FOnDeath OnDeath;
};
