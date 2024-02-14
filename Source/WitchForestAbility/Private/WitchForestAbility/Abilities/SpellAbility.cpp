// Copyright (c) 2024 Stephen Melnick


#include "WitchForestAbility/Abilities/SpellAbility.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/PlayerState.h"

#include "WitchForestAbility.h"

void USpellAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!ProjectileClass)
	{
		UE_LOG(LogWitchForestAbility, Warning, TEXT("Failed to spawn projectile, Projectile Class is invalid."));
		return;
	}
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	AActor* OwnerActor = ActorInfo->OwnerActor.Get();
	if (APlayerState* PlayerState = Cast<APlayerState>(OwnerActor))
	{
		OwnerActor = PlayerState->GetPawn();
	}
	FTransform ProjectileTransform = OwnerActor->GetActorTransform();
	AActor* Projectile = World->SpawnActor(ProjectileClass.Get(), &ProjectileTransform);
	if (!Projectile)
	{
		UE_LOG(LogWitchForestAbility, Warning, TEXT("Failed to spawn projectile."));
		return;
	}
	Projectile->OnActorHit.AddDynamic(this, &USpellAbility::ProjectileHit);
}

void USpellAbility::ProjectileHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	FGameplayEffectSpecHandle SpellEffectHandle = MakeOutgoingGameplayEffectSpec(SpellEffect);
	FGameplayAbilityTargetDataHandle TargetDataHandle;
	FGameplayAbilityTargetData* TargetData = new FGameplayAbilityTargetData();
	TargetData->ReplaceHitWith(OtherActor, &Hit);
	TargetDataHandle.Add(TargetData);
	ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, SpellEffectHandle, TargetDataHandle);
}
