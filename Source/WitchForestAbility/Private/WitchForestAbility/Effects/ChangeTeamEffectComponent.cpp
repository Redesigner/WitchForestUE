// Copyright (c) 2024 Stephen Melnick


#include "WitchForestAbility/Effects/ChangeTeamEffectComponent.h"

#include "GameplayEffect.h"
#include "AbilitySystemComponent.h"
#include "Logging/StructuredLog.h"

#include "WitchForestAbility.h"

bool UChangeTeamEffectComponent::OnActiveGameplayEffectAdded(FActiveGameplayEffectsContainer& ActiveGEContainer, FActiveGameplayEffect& ActiveGE) const
{
	if (ActiveGEContainer.IsNetAuthority())
	{
		ActiveGE.EventSet.OnEffectRemoved.AddUObject(this, &ThisClass::OnActiveGameplayEffectRemoved);
		ActiveGE.EventSet.OnInhibitionChanged.AddUObject(this, &ThisClass::OnInhibitionChanged);
	}

	// SetOverrideTeam(ActiveGE.Handle);
	return true;
}

void UChangeTeamEffectComponent::OnActiveGameplayEffectRemoved(const FGameplayEffectRemovalInfo& RemovalInfo) const
{
	EndOverrideTeam(RemovalInfo.ActiveEffect->Handle);
}

void UChangeTeamEffectComponent::OnInhibitionChanged(FActiveGameplayEffectHandle ActiveGEHandle, bool bIsInhibited) const
{
	if (!bIsInhibited)
	{
		SetOverrideTeam(ActiveGEHandle);
	}
	else
	{
		EndOverrideTeam(ActiveGEHandle);
	}
}

void UChangeTeamEffectComponent::SetOverrideTeam(FActiveGameplayEffectHandle ActiveGEHandle) const
{
	UAbilitySystemComponent* ASC = ActiveGEHandle.GetOwningAbilitySystemComponent();
	if (!ASC)
	{
		return;
	}

	if (IWitchForestTeamAgentInterface* TeamInterface = Cast<IWitchForestTeamAgentInterface>(ASC->GetOwnerActor()))
	{
		UE_LOGFMT(LogWitchForestAbility, Display, "ChangeTeamEffect overriding team of '{InterfaceActorName}'.", ASC->GetOwnerActor()->GetName());
		TeamInterface->OverrideTeam(NewTeam);
	}
}

void UChangeTeamEffectComponent::EndOverrideTeam(FActiveGameplayEffectHandle ActiveGEHandle) const
{
	UAbilitySystemComponent* ASC = ActiveGEHandle.GetOwningAbilitySystemComponent();
	if (!ASC)
	{
		return;
	}

	if (IWitchForestTeamAgentInterface* TeamInterface = Cast<IWitchForestTeamAgentInterface>(ASC->GetOwnerActor()))
	{
		UE_LOGFMT(LogWitchForestAbility, Display, "ChangeTeamEffect clearing override team of '{InterfaceActorName}'.", ASC->GetOwnerActor()->GetName());
		TeamInterface->OverrideTeam(EWitchForestTeam::Unaffiliated);
	}
}
