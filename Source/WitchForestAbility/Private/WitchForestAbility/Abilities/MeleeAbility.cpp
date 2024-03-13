// Copyright (c) 2024 Stephen Melnick


#include "WitchForestAbility/Abilities/MeleeAbility.h"

#include "WitchForestGame/Character/Components/MeleeComponent.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h" 
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"


void UMeleeAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!MeleeAnimation)
	{
		return;
	}

	APawn* Owner = Cast<APawn>(ActorInfo->AvatarActor);
	if (!Owner)
	{
		return;
	}

	UMeleeComponent* Melee = Owner->GetComponentByClass<UMeleeComponent>();
	if (!Melee)
	{
		return;
	}

	MeleeComponent = Melee;
	ActorHitDelegateHandle = MeleeComponent->OnActorHit.AddUObject(this, &ThisClass::OnEnemyHit);

	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("MeleeAnimation"), MeleeAnimation);
	// PlayMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::ForceEndAbility);
	PlayMontageTask->OnCompleted.AddDynamic(this, &ThisClass::ForceEndAbility);
	PlayMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::ForceEndAbility);
	PlayMontageTask->OnCancelled.AddDynamic(this, &ThisClass::ForceEndAbility);
	PlayMontageTask->ReadyForActivation();
	// This many delegates may be overkill, but I'm not sure if there is a delegate that will *always* get called
}

void UMeleeAbility::ForceEndAbility()
{
	if (MeleeComponent.IsValid())
	{
		MeleeComponent->OnActorHit.Remove(ActorHitDelegateHandle);
		MeleeComponent->DestroyAllHitboxes();
	}

	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void UMeleeAbility::OnEnemyHit(AActor* Enemy)
{
	IAbilitySystemInterface* ActorAbility = Cast<IAbilitySystemInterface>(Enemy);
	if (!ActorAbility)
	{
		return;
	}

	auto Predicate = [Enemy](const TWeakObjectPtr<AActor>& OldEnemy) { return Enemy == OldEnemy; };
	if (DamagedActors.ContainsByPredicate(Predicate))
	{
		return;
	}

	UAbilitySystemComponent* EnemyASC = ActorAbility->GetAbilitySystemComponent();
	if (!EnemyASC)
	{
		return;
	}

	for (TSubclassOf<UGameplayEffect> GameplayEffectClass : EffectsToApply)
	{
		FGameplayEffectSpecHandle GameplayEffectToApply = MakeOutgoingGameplayEffectSpec(GameplayEffectClass);
		EnemyASC->ApplyGameplayEffectSpecToSelf(*GameplayEffectToApply.Data.Get());
	}
	DamagedActors.Add(Enemy);
}
