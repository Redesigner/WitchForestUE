// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DropTableComponent.generated.h"

class UDropTable;

UCLASS( ClassGroup=(Components), meta=(BlueprintSpawnableComponent) )
class WITCHFORESTGAME_API UDropTableComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Drops, meta = (AllowPrivateAccess = true))
	TObjectPtr<UDropTable> DropTable;

public:	
	UDropTableComponent();

	void DropItems();

private:
	FVector GetLaunchVector() const;
};
