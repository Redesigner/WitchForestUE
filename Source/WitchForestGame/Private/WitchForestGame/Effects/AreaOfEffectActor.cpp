// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Effects/AreaOfEffectActor.h"

#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"

#include "WitchForestGame/Effects/EffectApplicationComponent.h"

AAreaOfEffectActor::AAreaOfEffectActor()
{
	EffectApplication = CreateDefaultSubobject<UEffectApplicationComponent>(TEXT("EffectApplication"));
}

void AAreaOfEffectActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	FHitResult FakeHit = FHitResult(OtherActor, nullptr, GetActorLocation(), FVector());
	
	TArray<FActiveGameplayEffectHandle> AppliedEffects = EffectApplication->ApplyGameplayEffectsToTarget(OtherActor, FakeHit);
	if (AppliedEffects.IsEmpty())
	{
		return;
	}

	AppliedEffectMap.Add(OtherActor, AppliedEffects);
}

void AAreaOfEffectActor::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	RemoveEffectsFromActor(OtherActor);
}

void AAreaOfEffectActor::Destroyed()
{
	TSet<FObjectKey> AffectedActors;
	AppliedEffectMap.GetKeys(AffectedActors);
	for (FObjectKey ObjectKey : AffectedActors)
	{
		AActor* AffectedActor = Cast<AActor>(ObjectKey.ResolveObjectPtr());
		if (!AffectedActor)
		{
			continue;
		}

		RemoveEffectsFromActor(AffectedActor);
	}
}

void AAreaOfEffectActor::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer = GameplayTags;
}

void AAreaOfEffectActor::RemoveEffectsFromActor(AActor* Actor)
{
	IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(Actor);
	if (!AbilitySystemInterface)
	{
		return;
	}

	UAbilitySystemComponent* ASC = AbilitySystemInterface->GetAbilitySystemComponent();
	if (!ASC)
	{
		return;
	}

	TArray<FActiveGameplayEffectHandle>* EffectsToRemove = AppliedEffectMap.Find(Actor);
	if (!EffectsToRemove)
	{
		return;
	}

	for (FActiveGameplayEffectHandle& EffectToRemove : *EffectsToRemove)
	{
		// Catch in case the ASC became invalidated by one of our effects
		if (!ASC)
		{
			return;
		}
		ASC->RemoveActiveGameplayEffect(EffectToRemove);
	}
}
