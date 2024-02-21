// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Character/Enemies/Enemy.h"

#include "WitchForestGame/Character/Enemies/EnemyMovementComponent.h"
#include "WitchForestGame/AI/EnemyAIController.h"
#include "WitchForestGame/Character/Components/DropTableComponent.h"
#include "WitchForestAbility/WitchForestASC.h"
#include "WitchForestAbility/Attributes/BaseAttributeSet.h"

#include "Components/CapsuleComponent.h"

AEnemy::AEnemy(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	RootComponent = Capsule;

	MovementComponent = CreateDefaultSubobject<UEnemyMovementComponent>(TEXT("Movement Component"));
	// MovementComponent->SetUpdatedComponent(RootComponent);

	AbilitySystem = CreateDefaultSubobject<UWitchForestASC>(TEXT("AbilitySystem"));
	AttributeSet = CreateDefaultSubobject<UBaseAttributeSet>(TEXT("Character attribute set"));

	AbilitySystem->OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &AEnemy::GameplayEffectApplied);

	DropTableComponent = CreateDefaultSubobject<UDropTableComponent>(TEXT("DropTableComponent"));
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

void AEnemy::MoveForward()
{
	AddMovementInput(GetActorForwardVector());
}

UCapsuleComponent* AEnemy::GetCapsule() const
{
	return Capsule;
}

void AEnemy::GameplayEffectApplied(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& GameplayEffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle)
{
	for (const FGameplayEffectModifiedAttribute& ModifiedAttribute : GameplayEffectSpec.ModifiedAttributes)
	{
		if (!bAlive)
		{
			return;
		}
		if (ModifiedAttribute.Attribute == AttributeSet->GetHealthAttribute())
		{
			AActor* Damager = GameplayEffectSpec.GetContext().GetInstigator();
			FHitResult Hit = *GameplayEffectSpec.GetContext().GetHitResult();
			OnTakeDamage.Broadcast(Damager, Hit);
			float NewHealth = ModifiedAttribute.Attribute.GetNumericValue(AttributeSet);
			if (NewHealth <= 0.0f)
			{
				bAlive = false;
				DropTableComponent->DropItems();
				SetLifeSpan(2.0f);
				OnDeath.Broadcast();
			}
		}
	}
}

UAbilitySystemComponent* AEnemy::GetAbilitySystemComponent() const
{
	return AbilitySystem;
}

UEnemyMovementComponent* AEnemy::GetEnemyMovementComponent() const
{
	return MovementComponent;
}

