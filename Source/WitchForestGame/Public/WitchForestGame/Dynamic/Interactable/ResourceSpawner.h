// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "WitchForestGame/Dynamic/Interactable/InteractableInterface.h"

#include "ResourceSpawner.generated.h"

class UDropTableComponent;

UCLASS()
class WITCHFORESTGAME_API AResourceSpawner : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> StaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDropTableComponent> DropTableComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning, meta = (AllowPrivateAccess = true))
	bool bDestroyOnSpawn = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning, meta = (AllowPrivateAccess = true))
	bool bActivateCooldownOnSpawn = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning, meta = (AllowPrivateAccess = true, EditCondition = bActivateCooldownOnSpawn))
	float CooldownTime = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spawning, meta = (AllowPrivateAccess = true, ClampMin = 0.0f))
	float HarvestHoldTime = 1.0f;

	UPROPERTY(ReplicatedUsing = OnRep_Cooldown, VisibleAnywhere, Category = Spawning, meta = (AllowPrivateAcces = true))
	bool bCooldown = false;


	FTimerHandle CooldownTimerHandle;


	AResourceSpawner();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void StartCooldown();

	void EndCooldown();

	void Interact_Implementation(AActor* Source) override;
	float GetRequiredHoldTime_Implementation() const override;
	bool CanInteract_Implementation(AActor* Source) const override;

	UFUNCTION()
	void OnRep_Cooldown(bool bOldCooldown);
};
