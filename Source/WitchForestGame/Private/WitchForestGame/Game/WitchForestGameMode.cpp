// Copyright (c) 2024 Stephen Melnick

#include "WitchForestGame/Game/WitchForestGameMode.h"

AWitchForestGameMode::AWitchForestGameMode()
{

}

UItemSet* AWitchForestGameMode::GetItemSet() const
{
    return CurrentItemSet;
}
