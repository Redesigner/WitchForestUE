// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"

#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"

#include "WitchForestLayout.generated.h"

class UWitchForestLayoutMap;
class UCommonActivatableWidget;
class UCommonActivatableWidgetContainerBase;
class UNotificationDataAsset;
class UNotificationWidget;
struct FWitchForestMessage;
struct FWitchForestUIMessage;

WITCHFORESTUI_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_SlotTag_Notification);

UCLASS()
class WITCHFORESTUI_API UWitchForestLayout : public UCommonUserWidget
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TObjectPtr<UWitchForestLayoutMap> LayoutMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Notifications, meta = (AllowPrivateAccess = true))
	TObjectPtr<UNotificationDataAsset> NotificationData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Notifications, meta = (AllowPrivateAccess = true))
	TSoftClassPtr<UNotificationWidget> NotificationWidgetClass;

	TMap<FGameplayTag, TObjectPtr<UCommonActivatableWidgetContainerBase>> LayerMap;

protected:
	UFUNCTION(BlueprintCallable)
	void RegisterLayer(UCommonActivatableWidgetContainerBase* Layer, UPARAM(meta = (Categories = "UI.SlotTag")) FGameplayTag SlotTag);

private:
	bool Initialize() override;

	void ReceiveMessage(FGameplayTag Channel, const FWitchForestUIMessage& Payload);

	void ReceiveNotification(FGameplayTag Channel, const FWitchForestMessage& Payload);

	UCommonActivatableWidget* PushWidgetToLayer(TSoftClassPtr<UCommonActivatableWidget> WidgetClass, FGameplayTag LayerTag);
};
