// Copyright (c) 2024 Stephen Melnick


#include "WitchForestUI/ViewModels/EffectDisplayViewModel.h"

#include "WitchForestUI.h"
#include "WitchForestGame/WitchForestGameplayTags.h"
#include "WitchForestGame/Game/WitchForestGameMode.h"

#include "AbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"

void UEffectDisplayViewModel::BindAbilitySystemComponent(UAbilitySystemComponent* AbilitySystemComponent)
{
	if (!AbilitySystemComponent)
	{
		UE_LOGFMT(LogWitchForestUI, Error, "EffectDisplayViewModel '{ModelInstanceName}' failed to bind to AbilitySystemComponent. The AbilitySystemComponent was invalid.", GetName());
		return;
	}

	// AbilitySystemComponent->RegisterGenericGameplayTagEvent().AddUObject(this, &ThisClass::GameplayEffectTagCountChanged);
	AbilitySystemComponent->OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &ThisClass::GameplayEffectApplied);
	OwningASC = AbilitySystemComponent;
}

void UEffectDisplayViewModel::GameplayEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& GameplayEffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle)
{
	if (!OwningASC.IsValid())
	{
		return;
	}

	if (!GameplayEffectSpec.Def)
	{
		return;
	}

	const UGameplayEffectUIDataComponent* UIDataComponent = GameplayEffectSpec.Def->FindComponent<UGameplayEffectUIDataComponent>();
	if (!UIDataComponent)
	{
		return;
	}

	FOnActiveGameplayEffectRemoved_Info* EffectRemovalDelegate = OwningASC->OnGameplayEffectRemoved_InfoDelegate(ActiveGameplayEffectHandle);
	if (!EffectRemovalDelegate)
	{
		UE_LOGFMT(LogWitchForestUI, Error, "EffectDisplayViewModel failed to bind to AbilitySystemComponent of '{ASCOwner}'. The EffectRemovalDelegate was null.",
			OwningASC->GetOwner() ? OwningASC->GetOwner()->GetName() : "Null");
		return;
	}

	UGameplayEffectUIDataWrapper* EffectUIData = NewObject<UGameplayEffectUIDataWrapper>();
	EffectUIData->Data = UIDataComponent->GetUIData();
	CurrentEffects.Add(EffectUIData);

	TWeakObjectPtr<UGameplayEffectUIDataWrapper> WeakRef = EffectUIData;

	BroadcastFieldValueChanged(ThisClass::FFieldNotificationClassDescriptor::CurrentEffects);

	EffectRemovalDelegate->AddWeakLambda(this,
		[this, WeakRef](const FGameplayEffectRemovalInfo& RemovalInfo)
		{
			if (WeakRef.IsValid())
			{
				CurrentEffects.Remove(WeakRef.Get());
			}
			BroadcastFieldValueChanged(ThisClass::FFieldNotificationClassDescriptor::CurrentEffects);
		});
}