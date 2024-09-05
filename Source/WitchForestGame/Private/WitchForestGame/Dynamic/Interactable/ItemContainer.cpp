// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Dynamic/Interactable/ItemContainer.h"

#include "Blueprint/UserWidget.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"

#include "WitchForestGame.h"
#include "WitchForestGame/Dynamic/Interactable/ActorParameterWidgetInterface.h"
#include "WitchForestGame/Dynamic/Pickup/Pickup.h"
#include "WitchForestGame/Game/WitchForestGameMode.h"
#include "WitchForestGame/Inventory/ItemSet.h"
#include "WitchForestGame/Math/WitchForestMath.h"

AItemContainer::AItemContainer()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	LaunchVectorArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("LaunchVector"));
	LaunchVectorArrow->SetupAttachment(RootComponent);
}

void AItemContainer::LaunchItemByIndex(int Index)
{
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

	if (!Items.IsValidIndex(Index))
	{
		UE_LOGFMT(LogWitchForestGame, Error, "ItemContainer '{ContainerName}' failed to launch item. The item index '{Index}' was invalid.", GetName(), Index);
		return;
	}

	FGameplayTag ItemTag = Items[Index];
	if (ItemTag == FGameplayTag::EmptyTag)
	{
		return;
	}

	FInventoryItemData ItemData;
	if (!ItemSet->FindItemDataForTag(ItemTag, ItemData))
	{
		UE_LOGFMT(LogWitchForestGame, Warning, "ItemContainer '{ContainerName}' failed to launch item. Could not find ItemData associated with ItemTag '{ItemTag}'", GetName(), ItemTag.ToString());
		return;
	}

	Items[Index] = FGameplayTag::EmptyTag;
	OnContentsChanged.Broadcast(Items);
	LaunchItem(ItemData.PickupClass);
}

void AItemContainer::LaunchItem(TSubclassOf<APickup> Item)
{
	if (!GetWorld())
	{
		UE_LOGFMT(LogWitchForestGame, Error, "ItemContainer '{ContainerName}' world was invalid.", GetName());
		return;
	}

	if (GetLocalRole() < ENetRole::ROLE_Authority)
	{
		return;
	}

	APickup* LaunchedItem = GetWorld()->SpawnActorDeferred<APickup>(Item, LaunchVectorArrow->GetComponentTransform());
	if (!LaunchedItem)
	{
		UE_LOGFMT(LogWitchForestGame, Error, "ItemContainer '{ContainerName}' failed to spawn item of class `{ItemClassName}'.", GetName(), Item->GetName());
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
	LaunchedItem->SetVelocity(MakeLaunchVector());
	LaunchedItem->FinishSpawning(LaunchVectorArrow->GetComponentTransform());
}

void AItemContainer::BeginPlay()
{
	Super::BeginPlay();
}

void AItemContainer::Interact_Implementation(AActor* Source)
{
	APawn* Pawn = Cast<APawn>(Source);
	if (!Pawn)
	{
		return;
	}

	// We can't launch widgets on non-local controllers!
	// Unreal will catch this, but it's better to avoid here
	APlayerController* PlayerController = Cast<APlayerController>(Pawn->GetController());
	if (!PlayerController || !PlayerController->IsLocalController())
	{
		return;
	}

	if (!ContainerWidgetClass)
	{
		return;
	}

	UUserWidget* SpawnedWidget = CreateWidget(PlayerController, ContainerWidgetClass);
	if (!SpawnedWidget)
	{
		UE_LOGFMT(LogWitchForestGame, Error, "ItemContainer '{ContainerName}' failed to launch widget. Widget of type '{ClassName}' was not created successfully.", GetName(), ContainerWidgetClass->GetName());
		return;
	}

	SpawnedWidget->AddToViewport();
	if (SpawnedWidget->Implements<UActorParameterWidgetInterface>())
	{
		IActorParameterWidgetInterface::Execute_AddActorParameter(SpawnedWidget, this);
	}
}

void AItemContainer::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	APickup* Pickup = Cast<APickup>(OtherActor);
	if (!Pickup)
	{
		return;
	}

	if (Pickup->bHeld)
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

	int NewItemIndex = 0;
	if (!HasEmptySlot(NewItemIndex))
	{
		Pickup->SetActorLocation(LaunchVectorArrow->GetComponentLocation());
		FVector LaunchVector = MakeLaunchVector();
		Pickup->SetVelocity(LaunchVector);
		Pickup->SetThrown(false);
		DrawDebugDirectionalArrow(GetWorld(), LaunchVectorArrow->GetComponentLocation(), LaunchVectorArrow->GetComponentLocation() + LaunchVector, 50.0f, FColor::Red, true, 2.0f);
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
				Items[NewItemIndex] = ItemTag;
				OnContentsChanged.Broadcast(Items);
			}
		}
		Pickup->Destroy();
	}
}

#if WITH_EDITOR
void AItemContainer::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, Items))
	{
		OnContentsChanged.Broadcast(Items);
	}
}
#endif

uint8 AItemContainer::CalculateEmptySlots() const
{
	uint8 CalculatedEmptySlotCount = 0;

	for (const FGameplayTag& ItemTag : Items)
	{
		if (ItemTag == FGameplayTag::EmptyTag)
		{
			CalculatedEmptySlotCount++;
		}
	}

	return CalculatedEmptySlotCount;
}

bool AItemContainer::HasEmptySlot(int& AvailableSlotIndexOut) const
{
	// Worst case is O(n), so this should be fine. (n is always going to be very small here anyways)
	for (int i = 0; i < Items.Num(); ++i)
	{
		if (Items[i] == FGameplayTag::EmptyTag)
		{
			AvailableSlotIndexOut = i;
			return true;
		}
	}

	AvailableSlotIndexOut = 0;
	return false;
}

FVector AItemContainer::MakeLaunchVector() const
{
	return WitchForestMath::MakeLaunchVector(MaxLaunchVelocity, MinLaunchVelocity, 0.0f, LaunchAngleRandomArc);
}
