// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Character/Enemies/Enemy.h"

#include "WitchForestGame/Character/Enemies/EnemyMovementComponent.h"
#include "WitchForestGame/Character/Enemies/EnemyAIController.h"
#include "WitchForestAbility/WitchForestASC.h"
#include "WitchForestAbility/Attributes/BaseAttributeSet.h"

#include "Components/CapsuleComponent.h"

AEnemy::AEnemy(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	RootComponent = Capsule;

	MovementComponent = CreateDefaultSubobject<UEnemyMovementComponent>(TEXT("Movement Component"));
	MovementComponent->SetUpdatedComponent(RootComponent);

	AbilitySystem = CreateDefaultSubobject<UWitchForestASC>(TEXT("AbilitySystem"));
	AttributeSet = CreateDefaultSubobject<UBaseAttributeSet>(TEXT("Character attribute set"));

	AbilitySystem->OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &AEnemy::GameplayEffectApplied);
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

UCapsuleComponent* AEnemy::GetCapsule() const
{
	return Capsule;
}

void AEnemy::GameplayEffectApplied(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& GameplayEffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle)
{
	for (const FGameplayEffectModifiedAttribute& ModifiedAttribute : GameplayEffectSpec.ModifiedAttributes)
	{
		if (ModifiedAttribute.Attribute == AttributeSet->GetHealthAttribute())
		{
			AActor* Damager = GameplayEffectSpec.GetContext().GetInstigator();
			OnTakeDamage.Execute(Damager);
		}
	}
}

