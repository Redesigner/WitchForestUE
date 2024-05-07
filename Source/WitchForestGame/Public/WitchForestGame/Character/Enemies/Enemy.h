// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayCueInterface.h"
#include "WitchForestGame/Game/WitchForestTeamAgentInterface.h"

#include "GameplayEffect.h"

#include "Enemy.generated.h"

class UWitchForestASC;
class UAbilitySystemComponent;
class UBaseAttributeSet;
class UDropTableComponent;
class UWitchForestAbilitySet;
class USkeletalMeshComponent;

UCLASS()
class WITCHFORESTGAME_API AEnemy : public ACharacter, public IAbilitySystemInterface, public IGameplayCueInterface, public IWitchForestTeamAgentInterface
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = true))
	TObjectPtr<UWitchForestASC> AbilitySystem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = true))
	TObjectPtr<UDropTableComponent> DropTableComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Abilities, meta = (AllowPrivateAccess = true))
	TArray<TObjectPtr<UWitchForestAbilitySet>> AbilitySets;

	UPROPERTY(Instanced, EditAnywhere, BlueprintReadOnly, Category = Attributes, meta = (AllowPrivateAccess = true))
	TObjectPtr<UBaseAttributeSet> AttributeSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attributes, meta = (ClampMin = 0.0f, AllowPrivateAccess = true))
	float StartingMaxHealth = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Enemy, meta = (Categories = "CreatureTag", AllowPrivateAccess = true))
	FGameplayTag CreatureTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool bAlive = true;


	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTakeDamage, AActor*, Source, FHitResult, Hit);

	DECLARE_MULTICAST_DELEGATE(FOnDeath)
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDeath);


	// Begin IWitchForestTeamAgentInterface
	void OverrideTeam(EWitchForestTeam NewTeam) override;
	void SetWitchForestTeam(EWitchForestTeam InTeam) override;
	EWitchForestTeam GetWitchForestTeam() const override;
	// End IWitchForestTeamAgentInterface

	AEnemy(const FObjectInitializer& ObjectInitializer);

	void BeginPlay() override;

	void Tick(float DeltaTime) override;

	void PossessedBy(AController* NewController) override;

	void Die();


public:
	UFUNCTION(BlueprintCallable)
	void MoveForward();

	void GameplayEffectApplied(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& GameplayEffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle);

	UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	FGameplayTag GetCreatureTag() const { return CreatureTag; }

	UPROPERTY(BlueprintAssignable)
	FOnTakeDamage OnTakeDamage;

	FOnDeath OnDeath;

	UPROPERTY(BlueprintAssignable)
	FDeath Death;
};
