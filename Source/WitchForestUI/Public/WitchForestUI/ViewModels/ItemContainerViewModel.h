// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"

#include "WitchForestUI/ViewModels/InventoryItemUIData.h"

#include "ItemContainerViewModel.generated.h"

class AItemContainer;

UCLASS()
class WITCHFORESTUI_API UItemContainerViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, FieldNotify, meta = (AllowPrivateAccess = true))
	TArray<TObjectPtr<UInventoryItemUIData>> ContainerItems;

public:
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, meta = (AllowPrivateAccess = true))
	int SelectedIndex = 0;

	UFUNCTION(BlueprintCallable)
	void BindItemContainer(AItemContainer* ItemContainer);

	UFUNCTION(BlueprintCallable)
	void RemoveItem();

	UFUNCTION(BlueprintCallable)
	void SetSelectedIndex(int32 Index) { SelectedIndex = Index; }

private:
	void ContentsChanged(const TArray<FGameplayTag>& Contents);

	TWeakObjectPtr<AItemContainer> BoundContainer;
};
