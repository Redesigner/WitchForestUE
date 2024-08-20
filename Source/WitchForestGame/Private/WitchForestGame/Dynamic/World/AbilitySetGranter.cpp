// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Dynamic/World/AbilitySetGranter.h"

#include "AbilitySystemInterface.h"

#include "WitchForestAbility/WitchForestASC.h"
#include "WitchForestAbility/Abilities/WitchForestAbilitySet.h"
#include "WitchForestGame/Character/Witch/Witch.h"
#include "WitchForestGame/Character/WitchPlayerState.h"

AAbilitySetGranter::AAbilitySetGranter()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AAbilitySetGranter::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (IAbilitySystemInterface* AbilityInterface = Cast<IAbilitySystemInterface>(OtherActor))
	{
		if (UWitchForestASC* ASC = Cast<UWitchForestASC>(AbilityInterface->GetAbilitySystemComponent()))
		{
			GrantAbilitiesTo(ASC);
		}
	}
}

void AAbilitySetGranter::GrantAbilitiesTo(UWitchForestASC* ASC)
{
	ASC->ClearTemporaryAbilities();

	for (const UWitchForestAbilitySet* AbilitySet : AbilitySets)
	{
		if (!AbilitySet)
		{
			continue;
		}

		ASC->GrantTemporaryAbilities(AbilitySet);
	}

	if (AWitch* Witch = Cast<AWitch>(ASC->GetAvatarActor()))
	{
		if (bSetWitchColor)
		{
			Witch->SetPlayerColor(WitchColor);

			if (AWitchPlayerState* WitchPlayerState = Cast<AWitchPlayerState>(Witch->GetPlayerState()))
			{
				WitchPlayerState->SetColor(WitchColor);
			}
		}
	}
}
