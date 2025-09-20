// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "ActorPreviewComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WITCHFORESTGAME_API UActorPreviewComponent : public USceneComponent
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess))
	TSubclassOf<AActor> PreviewActorClass;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta = (AllowPrivateAccess))
	TObjectPtr<AActor> PreviewActor;

public:
	void SetPreviewClass(const TSubclassOf<AActor>& PreviewClass);
	
private:
	
#if WITH_EDITOR
	void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	void SpawnPreviewActor();	
};
