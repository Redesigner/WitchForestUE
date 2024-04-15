// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Character/Enemies/Enemy.h"

#include "WitchForestGame/Character/Enemies/EnemyMovementComponent.h"
#include "WitchForestGame/AI/EnemyAIController.h"
#include "WitchForestGame/Character/Components/DropTableComponent.h"
#include "WitchForestGame/Character/Witch/Components/WitchMovementComponent.h"
#include "WitchForestAbility/WitchForestASC.h"
#include "WitchForestAbility/Attributes/BaseAttributeSet.h"
#include "WitchForestAbility/Abilities/WitchForestAbilitySet.h"

#include "Components/CapsuleComponent.h"


AEnemy::AEnemy(const FObjectInitializer& ObjectInitializer) :
	ACharacter(ObjectInitializer.SetDefaultSubobjectClass(ACharacter::CharacterMovementComponentName, UWitchMovementComponent::StaticClass()))
{
	AbilitySystem = CreateDefaultSubobject<UWitchForestASC>(TEXT("AbilitySystem"));
	AttributeSet = CreateDefaultSubobject<UBaseAttributeSet>(TEXT("Character attribute set"));
	AbilitySystem->OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &AEnemy::GameplayEffectApplied);

	DropTableComponent = CreateDefaultSubobject<UDropTableComponent>(TEXT("DropTableComponent"));
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	for (const UWitchForestAbilitySet* AbilitySet : AbilitySets)
	{
		if (!AbilitySet)
		{
			continue;
		}
		AbilitySet->GiveToAbilitySystem(AbilitySystem, nullptr);
	}

	AbilitySystem->SetAvatarActor(this);
	AttributeSet->SetMaxHealth(StartingMaxHealth);
	AttributeSet->SetHealth(StartingMaxHealth);
}


void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}


void AEnemy::Die()
{
	bAlive = false;
	DropTableComponent->DropItems();
	AbilitySystem->CancelAllAbilities();
	SetLifeSpan(2.0f);
	if (UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(GetMovementComponent()))
	{
		MovementComponent->SetMovementMode(EMovementMode::MOVE_None);
	}

	OnDeath.Broadcast();
	// Separate Dynamic delegate?
	Death.Broadcast();
}


void AEnemy::MoveForward()
{
	AddMovementInput(GetActorForwardVector());
}

void AEnemy::GameplayEffectApplied(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& GameplayEffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle)
{
	if (!AttributeSet)
	{
		return;
	}

	for (const FGameplayEffectModifiedAttribute& ModifiedAttribute : GameplayEffectSpec.ModifiedAttributes)
	{
		if (!bAlive)
		{
			return;
		}

		if (ModifiedAttribute.Attribute == AttributeSet->GetHealthAttribute())
		{
			AActor* Damager = GameplayEffectSpec.GetContext().GetInstigator();
			if (GameplayEffectSpec.GetContext().GetHitResult())
			{
				FHitResult Hit = *GameplayEffectSpec.GetContext().GetHitResult();
				OnTakeDamage.Broadcast(Damager, Hit);
			}
			else
			{
				OnTakeDamage.Broadcast(Damager, FHitResult());
			}

			float NewHealth = ModifiedAttribute.Attribute.GetNumericValue(AttributeSet);
			if (NewHealth <= 0.0f)
			{
				Die();
			}
		}
	}
}


UAbilitySystemComponent* AEnemy::GetAbilitySystemComponent() const
{
	return AbilitySystem;
}


void AEnemy::OverrideTeam(EWitchForestTeam NewTeam)
{
	if (IWitchForestTeamAgentInterface* TeamAgent = Cast<IWitchForestTeamAgentInterface>(GetController()))
	{
		TeamAgent->OverrideTeam(NewTeam);
	}
}


void AEnemy::SetWitchForestTeam(EWitchForestTeam InTeam)
{
	if (IWitchForestTeamAgentInterface* TeamAgent = Cast<IWitchForestTeamAgentInterface>(GetController()))
	{
		TeamAgent->SetWitchForestTeam(InTeam);
	}
}


EWitchForestTeam AEnemy::GetWitchForestTeam() const
{
	if (IWitchForestTeamAgentInterface* TeamAgent = Cast<IWitchForestTeamAgentInterface>(GetController()))
	{
		return TeamAgent->GetWitchForestTeam();
	}

	return EWitchForestTeam::Unaffiliated;
}