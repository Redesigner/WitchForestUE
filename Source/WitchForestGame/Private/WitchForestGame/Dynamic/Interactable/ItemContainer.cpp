// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Dynamic/Interactable/ItemContainer.h"

#include "Components/ArrowComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"
#include "Net/UnrealNetwork.h"

#include "WitchForestGame.h"
#include "WitchForestGame/WitchForestGameplayTags.h"
#include "WitchForestGame/Dynamic/Pickup/Pickup.h"
#include "WitchForestGame/Dynamic/World/ActorPreviewComponent.h"
#include "WitchForestGame/Game/WitchForestGameMode.h"
#include "WitchForestGame/Game/WitchForestGameState.h"
#include "WitchForestGame/Inventory/ItemSet.h"
#include "WitchForestGame/Math/WitchForestMath.h"
#include "WitchForestGame/Messages/WitchForestMessage.h"
#include "WitchForestGame/Network/ForwardNetworkRPCComponent.h"

AItemContainer::AItemContainer()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	LaunchVectorArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("LaunchVector"));
	LaunchVectorArrow->SetupAttachment(RootComponent);
}

void AItemContainer::RequestLaunchItemByIndex(APlayerController* Requester, int Index)
{
	if (HasAuthority())
	{
		LaunchItemByIndex(Index);
		return;
	}

	if (UForwardNetworkRPCComponent* Forwarder = Requester->FindComponentByClass<UForwardNetworkRPCComponent>())
	{
		Forwarder->ServerForwardRequest(this, Index);
	}
}

void AItemContainer::LaunchItemByIndex(int Index)
{
	AGameStateBase* GameState = UGameplayStatics::GetGameState(this);
	if (!GameState)
	{
		return;
	}

	// Get the *default* game mode by way of the game state, rather than the game mode directly
	// the game mode from GameplayStatics does not exist on clients
	const AWitchForestGameMode* GameMode = Cast<AWitchForestGameMode>(GameState->GetDefaultGameMode());
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

	//@TODO: Add item data as a param, since we've already looked it up and don't want to look it up again
	UpdateDisplaySlot(ItemSet, Index);
}

void AItemContainer::UpdateDisplays(const UItemSet* ItemSet)
{
	const int NumDisplays = Previews.Num();
	const int NumSlots = Items.Num();
	
	for (int i = 0; i < NumSlots && i < NumDisplays; ++i)
	{
		UpdateDisplaySlot(ItemSet, i);
	}
}

void AItemContainer::UpdateDisplaySlot(const UItemSet* ItemSet, const int Index)
{
	if (Index >= Previews.Num() || Index >= Items.Num())
	{
		return;
	}
	
	UActorPreviewComponent* PreviewComponent = Cast<UActorPreviewComponent>(Previews[Index].GetComponent(this));
	if (!PreviewComponent)
	{
		return;
	}

	const FGameplayTag& ItemTag = Items[Index];

	if (ItemTag == FGameplayTag::EmptyTag)
	{
		PreviewComponent->SetPreviewClass(nullptr);
		return;
	}

	FInventoryItemData SlottedItemData;
	if (!ItemSet->FindItemDataForTag(ItemTag, SlottedItemData))
	{
		return;
	}
	// Fallback to our pickup class
	PreviewComponent->SetPreviewClass(SlottedItemData.PreviewActor ? SlottedItemData.PreviewActor : SlottedItemData.PickupClass);
}

void AItemContainer::LaunchItem(TSubclassOf<APickup> Item)
{
	if (!GetWorld())
	{
		UE_LOGFMT(LogWitchForestGame, Error, "ItemContainer '{ContainerName}' world was invalid.", GetName());
		return;
	}

	//if (GetLocalRole() < ENetRole::ROLE_Authority)
	//{
	//	return;
	//}

	FTransform VectorTransform = LaunchVectorArrow->GetComponentTransform();
	FRotator VectorRotation = FRotator(0.0f, GetActorRotation().Yaw, 0.0f);
	VectorTransform.SetRotation(VectorRotation.Quaternion());
	
	APickup* LaunchedItem = GetWorld()->SpawnActorDeferred<APickup>(Item, VectorTransform);
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
	LaunchedItem->FinishSpawning(VectorTransform);
}

void AItemContainer::BeginPlay()
{
	Super::BeginPlay();

	if (AWitchForestGameMode* GameMode = Cast<AWitchForestGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		if (UItemSet* ItemSet = GameMode->GetItemSet())
		{
			UpdateDisplays(ItemSet);
		}
	}
}

void AItemContainer::Interact_Implementation(AActor* Source)
{
	APawn* Pawn = Cast<APawn>(Source);
	if (!Pawn)
	{
		return;
	}

	APlayerController* PlayerController = Cast<APlayerController>(Pawn->GetController());
	if (!PlayerController)
	{
		return;
	}

	//if (PlayerController->IsLocalController())
	//{
	//	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Pawn))
	//	{
	//		if (ASI->GetAbilitySystemComponent())
	//		{
	//			ASI->GetAbilitySystemComponent()
	//		}
	//	}
	//}

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());

	FWitchForestUIMessage NewMessage;
	NewMessage.MessageTag = WidgetMessageTag;
	NewMessage.RelevantActor = this;
	MessageSystem.BroadcastMessage(WitchForestGameplayTags::MessageChannel_UI, NewMessage);
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
		// DrawDebugDirectionalArrow(GetWorld(), LaunchVectorArrow->GetComponentLocation(), LaunchVectorArrow->GetComponentLocation() + LaunchVector, 50.0f, FColor::Red, true, 2.0f);
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
				UpdateDisplaySlot(ItemSet, NewItemIndex);
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

void AItemContainer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, Items);
}

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
	// Worst case is O(n), so this should be fine. (n is always going to be very small here anyway)
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
	FVector LaunchVector = WitchForestMath::MakeLaunchVector(MaxLaunchVelocity, MinLaunchVelocity, 0.0f, FMath::DegreesToRadians(LaunchAngleRandomArc));
	FRotator ArrowRotator = LaunchVectorArrow->GetComponentRotation();
	ArrowRotator.Pitch -= 90.0f;
	LaunchVector = ArrowRotator.RotateVector(LaunchVector);
	return LaunchVector;
}

void AItemContainer::OnRep_Items(TArray<FGameplayTag> OldTags)
{
	OnContentsChanged.Broadcast(Items);
}

void AItemContainer::HandleForwardedRPC(int Data)
{
	LaunchItemByIndex(Data);
}
