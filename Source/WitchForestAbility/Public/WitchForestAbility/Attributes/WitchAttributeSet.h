// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"

#include "AbilitySystemComponent.h"

#include "WitchAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class WITCHFORESTABILITY_API UWitchAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Harvesting, meta = (AllowPrivateAccess = true))
	FGameplayAttributeData HarvestPower;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Spellcasting, meta = (AllowPrivateAccess = true))
	FGameplayAttributeData SpellPower;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, ReplicatedUsing = OnRep_Mana, Category = Spellcasting, meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Mana;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, ReplicatedUsing = OnRep_MaxMana, Category = Spellcasting, meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxMana;

public:
	ATTRIBUTE_ACCESSORS(UWitchAttributeSet, HarvestPower)
	ATTRIBUTE_ACCESSORS(UWitchAttributeSet, SpellPower)
	ATTRIBUTE_ACCESSORS(UWitchAttributeSet, Mana);
	ATTRIBUTE_ACCESSORS(UWitchAttributeSet, MaxMana);

private:
	UWitchAttributeSet();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldMana);

	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana);
};
