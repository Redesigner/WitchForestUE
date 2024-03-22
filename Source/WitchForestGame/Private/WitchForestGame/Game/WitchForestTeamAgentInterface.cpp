// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Game/WitchForestTeamAgentInterface.h"

void IWitchForestTeamAgentInterface::SetGenericTeamId(const FGenericTeamId& TeamID)
{
	SetWitchForestTeam(GetTeamIdAsEnum(TeamID));
}

FGenericTeamId IWitchForestTeamAgentInterface::GetGenericTeamId() const
{
	return GetTeamIdAsEnum(GetWitchForestTeam());
}

void IWitchForestTeamAgentInterface::ConditionalBroadcastTeamChanged(TScriptInterface<IWitchForestTeamAgentInterface> This, EWitchForestTeam OldTeamID, EWitchForestTeam NewTeamID)
{
	if (OldTeamID != NewTeamID)
	{
		UObject* ThisObj = This.GetObject();
		if (FOnTeamChangedDelegate* OnTeamChangedDelegate = This.GetInterface()->GetOnTeamChangedDelegate())
		{
			OnTeamChangedDelegate->Broadcast(ThisObj, OldTeamID, NewTeamID);
		}
	}
}

EWitchForestTeam IWitchForestTeamAgentInterface::GetTeamIdAsEnum(FGenericTeamId GenericTeamID)
{
	if (GenericTeamID == FGenericTeamId::NoTeam)
	{
		return EWitchForestTeam::Unaffiliated;
	}

	if (GenericTeamID.GetId() >= EWitchForestTeam::Unaffiliated)
	{
		return EWitchForestTeam::Unaffiliated;
	}

	return EWitchForestTeam(GenericTeamID.GetId());
}