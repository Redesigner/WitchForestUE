// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameplayEffect.h"
#include "Enemy.generated.h"

class UCapsuleComponent;
class UEnemyMovementComponent;
class UWitchForestASC;
class UAbilitySystemComponent;
class UBaseAttributeSet;

UCLASS()
class WITCHFORESTGAME_API AEnemy : public APawn
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* Capsule;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	UEnemyMovementComponent* MovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	UWitchForestASC* AbilitySystem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attributes, meta = (AllowPrivateAccess = "true"))
	UBaseAttributeSet* AttributeSet;

public:
	AEnemy(const FObjectInitializer& ObjectInitializer);

	void BeginPlay() override;

	void Tick(float DeltaTime) override;

	void PossessedBy(AController* NewController) override;

	UCapsuleComponent* GetCapsule() const;

	void GameplayEffectApplied(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& GameplayEffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle);

	DECLARE_DELEGATE_OneParam(FOnTakeDamage, AActor*)
	FOnTakeDamage OnTakeDamage;
};
