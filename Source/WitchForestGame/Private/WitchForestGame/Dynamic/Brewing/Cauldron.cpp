// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Dynamic/Brewing/Cauldron.h"

#include "Components/SphereComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"
#include "Net/UnrealNetwork.h"

#include "WitchForestGame.h"
#include "WitchForestGame/Dynamic/Pickup/Pickup.h"
#include "WitchForestGame/Dynamic/Brewing/PotionRecipeSet.h"
#include "WitchForestGame/Game/WitchForestGameMode.h"
#include "WitchForestGame/Game/WitchForestGameState.h"
#include "WitchForestGame/Inventory/ItemSet.h"
#include "WitchForestGame/Messages/WitchForestMessage.h"
#include "WitchForestGame/Math/WitchForestMath.h"
#include "WitchForestGame/WitchForestGameplayTags.h"


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
		LaunchItem(ItemData.PickupClass);
	}
}

void ACauldron::LaunchItem(TSubclassOf<APickup> Item)
{
	if (!GetWorld())
	{
		UE_LOGFMT(LogWitchForestGame, Error, "Cauldron '{CauldronName}' world was invalid.", GetName());
		return;
	}

	if (GetLocalRole() < ENetRole::ROLE_Authority)
	{
		return;
	}

	APickup* LaunchedItem = GetWorld()->SpawnActorDeferred<APickup>(Item, CauldronVolume->GetComponentTransform());
	if (!LaunchedItem)
	{
		UE_LOGFMT(LogWitchForestGame, Error, "Cauldron '{CauldronName}' failed to spawn item of class `{ItemClassName}'.", GetName(), Item->GetName());
		return;
	}

	LaunchedItem->bHeld = true;
	FTimerHandle WaitTimer;
	FTimerDelegate WaitTimerDelegate;
	WaitTimerDelegate.BindLambda([LaunchedItem]()
		{
			LaunchedItem->bHeld = false;
		});

	GetWorld()->GetTimerManager().SetTimer(WaitTimer, WaitTimerDelegate, 0.25f, false, -1.0f);

	constexpr float MinPitch = FMath::DegreesToRadians(45.0f);
	constexpr float MaxPitch = FMath::DegreesToRadians(75.0f);
	const FVector LaunchVector = WitchForestMath::MakeLaunchVector(15000.0f, 30000.0f, MinPitch, MaxPitch);
	LaunchedItem->AddImpulse(LaunchVector);
	LaunchedItem->FinishSpawning(CauldronVolume->GetComponentTransform());
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

void ACauldron::ClearContents()
{
	HeldIngredients.Reset();
	OnContentsChanged.Broadcast();
}

void ACauldron::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, HeldIngredients);
}

void ACauldron::Interact_Implementation(AActor* Source)
{
	AWitchForestGameMode* GameMode = Cast<AWitchForestGameMode>(UGameplayStatics::GetGameMode(this));
	if (!GameMode)
	{
		return;
	}

	AWitchForestGameState* GameState = Cast<AWitchForestGameState>(UGameplayStatics::GetGameState(this));
	if (!GameState)
	{
		return;
	}

	UItemSet* ItemSet = GameMode->GetItemSet();
	if (!ItemSet)
	{
		return;
	}

	UPotionRecipeSet* RecipeBook = GameMode->GetRecipeBook();
	if (!RecipeBook)
	{
		UE_LOGFMT(LogWitchForestGame, Error, "Gamemode is missing a Recipe Book. Make sure one is set.");
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FPotionRecipe Recipe;
	if (!RecipeBook->FindRecipeFromIngredients(HeldIngredients, Recipe))
	{
		// Launch all of our ingredients back out
		TArray<FGameplayTag> PreviouslyHeldItems = HeldIngredients;
		for (const FGameplayTag& Item : PreviouslyHeldItems)
		{
			LaunchItem(Item, ItemSet);
		}
	}
	else
	{
		LaunchItem(Recipe.ResultItem, ItemSet);

		if (!GameState->HasLearnedRecipe(Recipe.RecipeTag))
		{
			UE_LOGFMT(LogWitchForestGame, Display, "Player '{PlayerName}' has discovered a new recipe for '{RecipeName}'.", Source->GetName(), Recipe.ResultItem.ToString());
			GameState->LearnRecipe(Recipe.RecipeTag);
			UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
			
			FWitchForestMessage NewMessage;
			NewMessage.Verb = WitchForestGameplayTags::Event_Discovery_Recipe;
			NewMessage.Data = Recipe.RecipeTag;
			NewMessage.Source = Source;
			MessageSystem.BroadcastMessage(WitchForestGameplayTags::Event_Discovery, NewMessage);
		}
	}

	ClearContents();
	// StartCooldown();
}

void ACauldron::VolumeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APickup* Pickup = Cast<APickup>(OtherActor);
	if (!Pickup)
	{
		return;
	}

	if (Pickup->bHeld)
	{
		return;
	}

	if (HeldIngredients.Num() >= Capacity)
	{
		TSubclassOf<APickup> PickupClass = Pickup->GetClass();
		Pickup->Destroy();

		FTimerHandle TimerHandle;
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda([this, PickupClass]()
			{
				LaunchItem(PickupClass);
			});

		GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 0.25f, false);
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
		if (!Pickup->IsFake())
		{
			Pickup->OnConsume();

			if (ItemTag != TAG_ItemEmpty)
			{
				HeldIngredients.Add(ItemTag);
				OnContentsChanged.Broadcast();
			}
		}

		Pickup->Destroy();
	}
}

FString ACauldron::GetContentsString() const
{
	FString Result;

	for (const FGameplayTag& Ingredient : HeldIngredients)
	{
		FString ShortName = Ingredient.ToString();
		ShortName.RemoveFromStart("ItemTag.");
		Result.Append(ShortName);
		Result.Append("\n");
	}
	return Result;
}
