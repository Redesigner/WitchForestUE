// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Dynamic/World/Trap.h"

#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"

#include "WitchForestGame/WitchForestGameplayTags.h"


ATrap::ATrap()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ATrap::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (ApplyEffectToActor(OtherActor) && bUsesCooldown)
	{
		StartCooldown();
	}
}

void ATrap::StartCooldown()
{
	bOnCooldown = true;
	GetWorld()->GetTimerManager().SetTimer(CooldownTimer, FTimerDelegate::CreateUObject(this, &ThisClass::EndCooldown), CooldownTime, false, -1.0f);
	OnStartCooldown();
}

void ATrap::EndCooldown()
{
	bOnCooldown = false;
	OnEndCooldown();

	ApplyEffectToOverlappingActors();
}

void ATrap::ApplyEffectToOverlappingActors()
{
	TSet<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors);
	for (AActor* OverlappingActor : OverlappingActors)
	{
		ApplyEffectToActor(OverlappingActor);
	}

	if (!OverlappingActors.IsEmpty())
	{
		StartCooldown();
	}
}

bool ATrap::ApplyEffectToActor(AActor* Target)
{
	if (bOnCooldown)
	{
		return false;
	}

	IAbilitySystemInterface* AbilityInterface = Cast<IAbilitySystemInterface>(Target);
	if (!AbilityInterface)
	{
		return false;
	}

	UAbilitySystemComponent* ASC = AbilityInterface->GetAbilitySystemComponent();
	if (!ASC)
	{
		return false;
	}

	if (!TrapEffect)
	{
		return false;
	}

	FGameplayEffectSpecHandle EffectSpec = ASC->MakeOutgoingSpec(TrapEffect, 1.0f, ASC->MakeEffectContext());
	if (!EffectSpec.Data.IsValid())
	{
		return false;
	}

	EffectSpec.Data->SetSetByCallerMagnitude(WitchForestGameplayTags::SetByCaller_Damage, -Damage);
	ASC->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data.Get());
	return true;
}
