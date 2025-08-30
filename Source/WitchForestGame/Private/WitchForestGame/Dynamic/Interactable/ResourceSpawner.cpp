// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Dynamic/Interactable/ResourceSpawner.h"

#include "WitchForestGame/Attributes/WitchAttributeSet.h"
#include "WitchForestGame.h"
#include "WitchForestGame/Character/Components/DropTableComponent.h"

#include "Net/UnrealNetwork.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Logging/StructuredLog.h"


AResourceSpawner::AResourceSpawner()
{
	bReplicates = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(RootComponent);

	DropTableComponent = CreateDefaultSubobject<UDropTableComponent>(TEXT("DropTable"));
}

void AResourceSpawner::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bCooldown);
}


void AResourceSpawner::Interact_Implementation(AActor* Source)
{
	if (bCooldown)
	{
		return;
	}

	float DropPower = 100.0f;
	if (IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(Source))
	{
		UAbilitySystemComponent* ASC = AbilitySystemInterface->GetAbilitySystemComponent();
		if (const UWitchAttributeSet* WitchAttributeSet = Cast<UWitchAttributeSet>(ASC->GetAttributeSet(UWitchAttributeSet::StaticClass())))
		{
			DropPower = WitchAttributeSet->GetHarvestPower();
			UE_LOGFMT(LogWitchForestGame, Display, "'{SourceName}' harvested resources from '{SelfName}', with a DropPower of {DropPower}.", Source ? Source->GetName() : "Null", GetName(), DropPower);
		}
	}
	DropTableComponent->DropItems(DropPower);

	if (bDestroyOnSpawn)
	{
		Destroy();
		return;
	}

	if (bActivateCooldownOnSpawn)
	{
		StartCooldown();
	}
}

float AResourceSpawner::GetRequiredHoldTime_Implementation() const
{
	return HarvestHoldTime;
}

bool AResourceSpawner::CanInteract_Implementation(AActor* Source) const
{
	return !bCooldown;
}

void AResourceSpawner::OnRep_Cooldown(bool bOldCooldown)
{
	if (bCooldown == bOldCooldown)
	{
		return;
	}

	StaticMesh->SetVisibility(!bCooldown, true);
}

void AResourceSpawner::StartCooldown()
{
	UWorld* World = GetWorld();
	check(World);
	FTimerManager& TimerManager = World->GetTimerManager();

	TimerManager.SetTimer(CooldownTimerHandle, FTimerDelegate::CreateUObject(this, &ThisClass::EndCooldown), CooldownTime, false);

	StaticMesh->SetVisibility(false, true);
	bCooldown = true;
}

void AResourceSpawner::EndCooldown()
{
	StaticMesh->SetVisibility(true, true);
	bCooldown = false;
}
