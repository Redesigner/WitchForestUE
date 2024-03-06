// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Dynamic/Brewing/Cauldron.h"

#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"
#include "Net/UnrealNetwork.h"

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


void ACauldron::LaunchItem(const FGameplayTag& Item, UItemSet* ItemSet)
{
	FInventoryItemData ItemData;
	if (ItemSet->FindItemDataForTag(Item, ItemData))
	{
		APickup* LaunchedItem = GetWorld()->SpawnActor<APickup>(ItemData.PickupClass, CauldronVolume->GetComponentLocation(), CauldronVolume->GetComponentRotation());
		if (!LaunchedItem)
		{
			UE_LOGFMT(LogWitchForestGame, Error, "Cauldron '{CauldronName}' failed to spawn item of class `{ItemClassName}', matching tag '{ItemTag}'.", GetName(), ItemData.PickupClass->GetName(), Item.GetTagName());
			return;
		}
		constexpr float MinPitch = FMath::DegreesToRadians(45.0f);
		constexpr float MaxPitch = FMath::DegreesToRadians(75.0f);
		const FVector LaunchVector = MakeLaunchVector(15000.0f, 30000.0f, MinPitch, MaxPitch);
		LaunchedItem->AddImpulse(LaunchVector);
	}
}

void ACauldron::StartCooldown()
{
	FTimerDelegate EndCooldown = FTimerDelegate::CreateLambda([this]()
		{
			CauldronVolume->SetGenerateOverlapEvents(true);
		});

	GetWorld()->GetTimerManager().SetTimer(CooldownTimerHandle, EndCooldown, 2.0f, false);
}

void ACauldron::OnRep_HeldIngredients(TArray<FGameplayTag> OldTags)
{
	OnContentsChanged.Broadcast();
}

void ACauldron::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, HeldIngredients);
}

void ACauldron::Interact(AActor* Source)
{
	if (!RecipeBook)
	{
		UE_LOGFMT(LogWitchForestGame, Error, "Cauldron '{CauldronName}' is missing a Recipe Book. Make sure one is set.", GetName());
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

	// Disable overlap events here, to prevent any side effects
	CauldronVolume->SetGenerateOverlapEvents(false);
	FGameplayTag RecipeResult = RecipeBook->MakeItem(HeldIngredients);
	if (RecipeResult == TAG_RecipeFailed)
	{
		TArray<FGameplayTag> PreviouslyHeldItems = HeldIngredients;
		for (const FGameplayTag& Item : PreviouslyHeldItems)
		{
			LaunchItem(Item, ItemSet);
		}
	}
	else
	{
		LaunchItem(RecipeResult, ItemSet);
	}

	HeldIngredients.Reset();
	OnContentsChanged.Broadcast();
	StartCooldown();
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

	if (HeldIngredients.Num() >= Capacity)
	{
		Pickup->SetVelocity(FVector::Zero());
		constexpr float MinPitch = FMath::DegreesToRadians(45.0f);
		constexpr float MaxPitch = FMath::DegreesToRadians(75.0f);
		const FVector LaunchVector = MakeLaunchVector(15000.0f, 30000.0f, MinPitch, MaxPitch);
		Pickup->AddImpulse(LaunchVector);
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
		OnContentsChanged.Broadcast();
		Pickup->Destroy();
	}
}

FString ACauldron::GetContentsString() const
{
	FString Result;

	for (const FGameplayTag& Ingredient : HeldIngredients)
	{
		Result.Append(Ingredient.GetTagName().ToString());
		Result.Append("\n");
	}
	return Result;
}
