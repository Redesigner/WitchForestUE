// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Notifies/AnimNotify_GameplayEvent.h"

#include "AbilitySystemInterface.h"

#include "WitchForestGame/WitchForestASC.h"

UAnimNotify_GameplayEvent::UAnimNotify_GameplayEvent()
{
#if WITH_EDITORONLY_DATA
	bShouldFireInEditor = false;
#endif
}

void UAnimNotify_GameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	IAbilitySystemInterface* OwnerAbilitySystemInterface = Cast<IAbilitySystemInterface>(MeshComp->GetOwner());
	if (!OwnerAbilitySystemInterface)
	{
		return;
	}

	UAbilitySystemComponent* ASC = OwnerAbilitySystemInterface->GetAbilitySystemComponent();
	if (!ASC)
	{
		return;
	}

	FGameplayEventData EventData;
	EventData.Instigator = MeshComp->GetOwner();
	EventData.EventTag = EventTag;
	EventData.Target = MeshComp->GetOwner();

	ASC->HandleGameplayEvent(EventTag, &EventData);
}
