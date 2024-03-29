// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Dynamic/World/AbilitySetGranter.h"

#include "AbilitySystemInterface.h"

#include "WitchForestAbility/WitchForestASC.h"
#include "WitchForestAbility/Abilities/WitchForestAbilitySet.h"

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
}
