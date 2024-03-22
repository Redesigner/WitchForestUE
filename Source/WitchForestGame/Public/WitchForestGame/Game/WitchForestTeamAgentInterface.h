// Copyright (c) 2024 Stephen Melnick

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"

#include "WitchForestTeamAgentInterface.generated.h"

UENUM(BlueprintType)
enum EWitchForestTeam
{
	Witches,
	Wild,
	Unaffiliated
};


template <typename InterfaceType> class TScriptInterface;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnTeamChangedDelegate, UObject*, ObjectChangingTeam, EWitchForestTeam, OldTeamID, EWitchForestTeam, NewTeamID);

UINTERFACE(meta = (CannotImplementInterfaceInBlueprint))
class WITCHFORESTGAME_API UWitchForestTeamAgentInterface : public UGenericTeamAgentInterface
{
	GENERATED_BODY()
};

// The Interface is not completed yet, it mostly shadows GenericTeamAgentInterface for now
class WITCHFORESTGAME_API IWitchForestTeamAgentInterface : public IGenericTeamAgentInterface
{
	GENERATED_BODY()

	// Begin IGenericTeamAgentInterface
	void SetGenericTeamId(const FGenericTeamId& TeamID) override final;
	FGenericTeamId GetGenericTeamId() const override final;
	// End IGenericTeamAgentInterface

	virtual FOnTeamChangedDelegate* GetOnTeamChangedDelegate() { return nullptr; }

	static void ConditionalBroadcastTeamChanged(TScriptInterface<IWitchForestTeamAgentInterface> This, EWitchForestTeam OldTeamID, EWitchForestTeam NewTeamID);

	static EWitchForestTeam GetTeamIdAsEnum(FGenericTeamId GenericTeamID);

public:
	virtual void OverrideTeam(EWitchForestTeam NewTeam) = 0;
	virtual void SetWitchForestTeam(EWitchForestTeam InTeam) = 0;
	virtual EWitchForestTeam GetWitchForestTeam() const = 0;
};