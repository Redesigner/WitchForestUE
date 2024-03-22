// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Character/Enemies/Enemy.h"

#include "WitchForestGame/Character/Enemies/EnemyMovementComponent.h"
#include "WitchForestGame/AI/EnemyAIController.h"
#include "WitchForestGame/Character/Components/DropTableComponent.h"
#include "WitchForestAbility/WitchForestASC.h"
#include "WitchForestAbility/Attributes/BaseAttributeSet.h"
#include "WitchForestAbility/Abilities/WitchForestAbilitySet.h"

#include "Components/CapsuleComponent.h"

AEnemy::AEnemy(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	RootComponent = Capsule;

	MovementComponent = CreateDefaultSubobject<UEnemyMovementComponent>(TEXT("Movement Component"));
	// MovementComponent->SetUpdatedComponent(RootComponent);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	Mesh->SetupAttachment(RootComponent);

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

bool AEnemy::IsPlayingRootMotion() const
{
	if (Mesh)
	{
		return Mesh->IsPlayingRootMotion();
	}
	return false;
}

USkeletalMeshComponent* AEnemy::GetSkeletalMesh() const
{
	return Mesh;
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