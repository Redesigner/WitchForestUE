// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Game/WitchForestCheatManager.h"

#include "WitchForestGame/WitchForestGameplayTags.h"
#include "WitchForestGame/Character/WitchPlayerController.h"
#include "WitchForestGame/Character/WitchPlayerState.h"

#include "WitchForestAbility/WitchForestASC.h"
#include "WitchForestAbility/Attributes/BaseAttributeSet.h"

void UWitchForestCheatManager::SetImmortal(bool bImmortal)
{
	AWitchPlayerController* WitchPlayerController = Cast<AWitchPlayerController>(GetOuterAPlayerController());
	if (!WitchPlayerController)
	{
		return;
	}

	AWitchPlayerState* WitchPlayerState = WitchPlayerController->GetPlayerState<AWitchPlayerState>();
	if (!WitchPlayerState)
	{
		return;
	}

	UWitchForestASC* ASC = WitchPlayerState->GetWitchForestASC();
	if (!ASC)
	{
		return;
	}

	if (bImmortal)
	{
		ASC->AddLooseGameplayTag(WitchForestGameplayTags::Cheat_Immortal);
	}
	else
	{
		ASC->RemoveLooseGameplayTag(WitchForestGameplayTags::Cheat_Immortal);
	}
}

void UWitchForestCheatManager::KillPlayer()
{
	AWitchPlayerController* WitchPlayerController = Cast<AWitchPlayerController>(GetOuterAPlayerController());
	if (!WitchPlayerController)
	{
		return;
	}

	AWitchPlayerState* WitchPlayerState = WitchPlayerController->GetPlayerState<AWitchPlayerState>();
	if (!WitchPlayerState)
	{
		return;
	}

	WitchPlayerState->GetAttributeSet()->KillOwner();
}
