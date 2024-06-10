// Copyright (c) 2024 Stephen Melnick


#include "WitchForestAbility/Attributes/BaseAttributeSet.h"

#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h" 
#include "Perception/AISense_Damage.h" 

#include "WitchForestGame/WitchForestGameplayTags.h"

UBaseAttributeSet::UBaseAttributeSet()
{
	MovementSpeedModifier.SetBaseValue(1.0f);
	MovementSpeedModifier.SetCurrentValue(1.0f);

	Health.SetBaseValue(100.0f);
	Health.SetCurrentValue(100.0f);

	MaxHealth.SetBaseValue(100.0f);
	MaxHealth.SetCurrentValue(100.0f);
}


void UBaseAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBaseAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
}

bool UBaseAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}

#if !UE_BUILD_SHIPPING
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		if (Data.Target.HasMatchingGameplayTag(WitchForestGameplayTags::Cheat_Immortal))
		{
			Data.EvaluatedData.Magnitude = 0.0f;
		}
	}
#endif // !UE_BUILD_SHIPPING

	HealthBeforeAttributeChange = GetHealth();
	MaxHealthBeforeAttributeChange = GetMaxHealth();

	return true;
}


void UBaseAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		// Make sure this damage wasn't self-inflicted
		if (Data.EvaluatedData.Magnitude > 0.0f && Data.EffectSpec.GetEffectContext().GetInstigatorAbilitySystemComponent() != GetOwningAbilitySystemComponent())
		{
			BroadcastDamageEventPerception(Data);
		}

		// Modify health, and clear the damage attribute
		const float NewHealth = FMath::Clamp(GetHealth() - GetDamage(), 0.0f, GetMaxHealth());
		SetHealth(NewHealth);
		SetDamage(0.0f);
	}

	if (Data.EvaluatedData.Attribute == GetDamageAttribute() || Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		if (GetHealth() <= 0.0f && GetHealth() != HealthBeforeAttributeChange)
		{
			UE_LOG(LogTemp, Display, TEXT("Character died."));
			OnDeath.Broadcast(Data.EffectSpec);

			FGameplayEventData Payload;
			Payload.EventTag = WitchForestGameplayTags::GameplayEvent_Death;
			Payload.Instigator = Data.EffectSpec.GetEffectContext().GetEffectCauser();
			Payload.Target = GetOwningAbilitySystemComponent()->GetOwnerActor();
			Payload.ContextHandle = Data.EffectSpec.GetEffectContext();

			GetOwningAbilitySystemComponent()->HandleGameplayEvent(Payload.EventTag, &Payload);
		}
	}
}


void UBaseAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, MaxHealth.GetCurrentValue());
		return;
	}

	if (Attribute == GetMaxHealthAttribute())
	{
		// If we decreased the max health, clamp our health so it isn't greater than max
		float CurrentHealth = Health.GetCurrentValue();
		if (NewValue < CurrentHealth)
		{
			Health.SetCurrentValue(NewValue);
		}
		return;
	}
}

void UBaseAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, MaxHealth.GetCurrentValue());
		return;
	}
}

void UBaseAttributeSet::BroadcastDamageEventPerception(const FGameplayEffectModCallbackData& Data)
{
	FVector DamageLocation;
	if (Data.EffectSpec.GetContext().HasOrigin())
	{
		DamageLocation = Data.EffectSpec.GetContext().GetOrigin();
	}
	else if (GetOwningAbilitySystemComponent() && GetOwningAbilitySystemComponent()->GetAvatarActor())
	{
		DamageLocation = GetOwningAbilitySystemComponent()->GetAvatarActor()->GetActorLocation();
	}

	UAISense_Damage::ReportDamageEvent(
		GetWorld(), GetOwningAbilitySystemComponent()->GetOwnerActor(), Data.EffectSpec.GetEffectContext().GetEffectCauser(),
		-Data.EvaluatedData.Magnitude, DamageLocation, DamageLocation
	);
}


void UBaseAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, Health, OldHealth);

	HealthBeforeAttributeChange = GetHealth();
}


void UBaseAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, MaxHealth, OldMaxHealth);

	MaxHealthBeforeAttributeChange = GetMaxHealth();
}