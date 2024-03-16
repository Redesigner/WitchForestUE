// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "AbilitySystemInterface.h"
#include "GameplayCueInterface.h"

#include "GameplayEffect.h"

#include "Enemy.generated.h"

class UCapsuleComponent;
class UEnemyMovementComponent;
class UWitchForestASC;
class UAbilitySystemComponent;
class UBaseAttributeSet;
class UDropTableComponent;
class UWitchForestAbilitySet;
class USkeletalMeshComponent;

UCLASS()
class WITCHFORESTGAME_API AEnemy : public APawn, public IAbilitySystemInterface, public IGameplayCueInterface
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = true))
	TObjectPtr<UCapsuleComponent> Capsule;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = true))
	TObjectPtr<UEnemyMovementComponent> MovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = true))
	TObjectPtr<USkeletalMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = true))
	TObjectPtr<UWitchForestASC> AbilitySystem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = true))
	TObjectPtr<UDropTableComponent> DropTableComponent;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Abilities, meta = (AllowPrivateAccess = true))
	TArray<TObjectPtr<UWitchForestAbilitySet>> AbilitySets;

	UPROPERTY(Instanced, EditAnywhere, BlueprintReadOnly, Category = Attributes, meta = (AllowPrivateAccess = true))
	TObjectPtr<UBaseAttributeSet> AttributeSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool bAlive = true;

public:
	AEnemy(const FObjectInitializer& ObjectInitializer);

	void BeginPlay() override;

	void Tick(float DeltaTime) override;

	void PossessedBy(AController* NewController) override;

	UFUNCTION(BlueprintCallable)
	void MoveForward();

	UCapsuleComponent* GetCapsule() const;

	void GameplayEffectApplied(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& GameplayEffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle);

	UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UEnemyMovementComponent* GetEnemyMovementComponent() const;

	bool IsPlayingRootMotion() const;

	USkeletalMeshComponent* GetSkeletalMesh() const;



	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTakeDamage, AActor*, Source, FHitResult, Hit);
	UPROPERTY(BlueprintAssignable)
	FOnTakeDamage OnTakeDamage;

	DECLARE_MULTICAST_DELEGATE(FOnDeath)
	FOnDeath OnDeath;
};
