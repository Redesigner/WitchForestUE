// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"

#include "GameplayTagContainer.h"

#include "WitchForestLayout.generated.h"

class UWitchForestLayoutMap;
class UCommonActivatableWidget;
class UCommonActivatableWidgetContainerBase;
struct FWitchForestUIMessage;

UCLASS()
class WITCHFORESTUI_API UWitchForestLayout : public UCommonUserWidget
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UWitchForestLayoutMap> LayoutMap;

	TMap<FGameplayTag, TObjectPtr<UCommonActivatableWidgetContainerBase>> LayerMap;

protected:
	UFUNCTION(BlueprintCallable)
	void RegisterLayer(UCommonActivatableWidgetContainerBase* Layer, UPARAM(meta = (Categories = "UI.SlotTag")) FGameplayTag SlotTag);

private:
	bool Initialize() override;

	void ReceiveMessage(FGameplayTag Channel, const FWitchForestUIMessage& Payload);

	UCommonActivatableWidget* PushWidgetToLayer(TSoftClassPtr<UCommonActivatableWidget> WidgetClass, FGameplayTag LayerTag);
};
