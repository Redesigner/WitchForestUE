// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Dynamic/Brewing/Cauldron.h"

#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"

#include "WitchForestGame.h"
#include "WitchForestGame/Dynamic/Pickup/Pickup.h"
#include "WitchForestGame/Dynamic/Brewing/PotionRecipeSet.h"
#include "WitchForestGame/Game/WitchForestGameMode.h"
#include "WitchForestGame/Inventory/ItemSet.h"


ACauldron::ACauldron()
{
	CauldronVolume = CreateDefaultSubobject<USphereComponent>(TEXT("CauldronVolume"));
	CauldronVolume->SetupAttachment(RootComponent);
	CauldronVolume->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::VolumeOverlap);
}


void ACauldron::Interact(AActor* Source)
{
	if (!RecipeBook)
	{
		UE_LOGFMT(LogWitchForestGame, Error, "Cauldron '{CauldronName}' is missing a Recipe Book. Make sure one is set.", GetName());
		return;
	}

	FGameplayTag RecipeResult = RecipeBook->MakeItem(HeldIngredients);
	HeldIngredients.Reset();
	if (RecipeResult == TAG_RecipeFailed)
	{
		return;
	}

	AWitchForestGameMode* GameMode = Cast<AWitchForestGameMode>(UGameplayStatics::GetGameMode(this));
	if (!GameMode)
	{
		return;
	}

	UItemSet* ItemSet = GameMode->GetItemSet();
	if (!ItemSet)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FInventoryItemData ItemData;
	if (ItemSet->FindItemDataForTag(RecipeResult, ItemData))
	{
		CauldronVolume->SetGenerateOverlapEvents(false);
		FTimerDelegate EndCooldown = FTimerDelegate::CreateLambda([this]()
			{
				CauldronVolume->SetGenerateOverlapEvents(true);
			});

		World->GetTimerManager().SetTimer(CooldownTimerHandle, EndCooldown, 2.0f, false);

		APickup* BrewedItem = World->SpawnActor<APickup>(ItemData.PickupClass, CauldronVolume->GetComponentLocation(), CauldronVolume->GetComponentRotation());
		constexpr float MinPitch = FMath::DegreesToRadians(45.0f);
		constexpr float MaxPitch = FMath::DegreesToRadians(75.0f);
		const FVector LaunchVector = MakeLaunchVector(15000.0f, 30000.0f, MinPitch, MaxPitch);
		BrewedItem->AddImpulse(LaunchVector);
	}
}

FVector ACauldron::MakeLaunchVector(float MaxSpeed, float MinSpeed, float MinPitch, float MaxPitch) const
{
	const float LaunchPitch = FMath::FRandRange(MinPitch, MaxPitch);
	const float LaunchYaw = FMath::FRandRange(-PI, PI);
	const float LaunchSpeed = FMath::FRandRange(MinSpeed, MaxSpeed);
	const float CircleRadius = FMath::Cos(LaunchPitch);
	return FVector(
		FMath::Cos(LaunchYaw) * CircleRadius,
		FMath::Sin(LaunchYaw) * CircleRadius,
		FMath::Sin(LaunchPitch)) * LaunchSpeed;
}


void ACauldron::VolumeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APickup* Pickup = Cast<APickup>(OtherActor);
	if (!Pickup || Pickup->bHeld)
	{
		return;
	}

	AWitchForestGameMode* GameMode = Cast<AWitchForestGameMode>(UGameplayStatics::GetGameMode(this));
	if (!GameMode)
	{
		return;
	}

	UItemSet* ItemSet = GameMode->GetItemSet();
	if (!ItemSet)
	{
		return;
	}

	FGameplayTag ItemTag;
	if (ItemSet->FindItemTagFromClass(Pickup->GetClass(), ItemTag))
	{
		HeldIngredients.Add(ItemTag);
		Pickup->Destroy();
	}
}
