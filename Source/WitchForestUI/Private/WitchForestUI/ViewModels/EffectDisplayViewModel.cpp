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
		UE_LOGFMT(LogWitchForestUI, Display, "GameplayEffect '{EffectName}' did not have a valid UIDataComponent", GameplayEffectSpec.Def->GetName());
		return;
	}

	FOnActiveGameplayEffectRemoved_Info* EffectRemovalDelegate = OwningASC->OnGameplayEffectRemoved_InfoDelegate(ActiveGameplayEffectHandle);
	if (!EffectRemovalDelegate)
	{
		UE_LOGFMT(LogWitchForestUI, Error, "EffectDisplayViewModel failed to bind to AbilitySystemComponent of '{ASCOwner}'. The EffectRemovalDelegate was null. An icon will not be displayed for '{EffectName}'.",
			OwningASC->GetOwner() ? OwningASC->GetOwner()->GetName() : "Null", GameplayEffectSpec.Def->GetName());
		return;
	}

	UGameplayEffectUIDataWrapper* EffectUIData;
	if (!TryIncrementIconCount(UIDataComponent, EffectUIData))
	{
		EffectUIData = NewObject<UGameplayEffectUIDataWrapper>();
		EffectUIData->Data = UIDataComponent->GetUIData();
		CurrentEffects.Add(EffectUIData);
	}
	TWeakObjectPtr<UGameplayEffectUIDataWrapper> WeakRef = EffectUIData;

	BroadcastFieldValueChanged(ThisClass::FFieldNotificationClassDescriptor::CurrentEffects);

	EffectRemovalDelegate->AddWeakLambda(this,
		[this, WeakRef](const FGameplayEffectRemovalInfo& RemovalInfo)
		{
			if (WeakRef.IsValid())
			{
				WeakRef->Count--;
				if (WeakRef->Count <= 0)
				{
					CurrentEffects.Remove(WeakRef.Get());
				}

				BroadcastFieldValueChanged(ThisClass::FFieldNotificationClassDescriptor::CurrentEffects);
			}
		});
}

bool UEffectDisplayViewModel::TryIncrementIconCount(const UGameplayEffectUIDataComponent* UIDataComponent, UGameplayEffectUIDataWrapper*& DataWrapper)
{
	for (UGameplayEffectUIDataWrapper* CurrentEffectData : CurrentEffects)
	{
		if (CurrentEffectData->Data.Icon == UIDataComponent->GetUIData().Icon)
		{
			DataWrapper = CurrentEffectData;
			CurrentEffectData->Count++;
			return true;
		}
	}
	return false;
}
