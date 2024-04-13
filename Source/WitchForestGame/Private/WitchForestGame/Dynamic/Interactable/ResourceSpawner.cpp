// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Dynamic/Interactable/ResourceSpawner.h"

#include "WitchForestGame/Character/Components/DropTableComponent.h"

#include "Net/UnrealNetwork.h"


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


void AResourceSpawner::Interact(AActor* Source)
{
	if (bCooldown)
	{
		return;
	}

	DropTableComponent->DropItems();

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

float AResourceSpawner::GetRequiredHoldTime() const
{
	return HarvestHoldTime;
}

bool AResourceSpawner::CanInteract(AActor* Source) const
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
