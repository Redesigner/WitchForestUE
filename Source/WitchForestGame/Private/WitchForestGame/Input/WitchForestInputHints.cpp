// Copyright (c) 2024 Stephen Melnick


#include "WitchForestGame/Input/WitchForestInputHints.h"

FName UWitchForestInputHints::FindGamepadButtonNameForInputTag(const FGameplayTag& InputTag) const
{
	for (const FWitchForestInputHint& InputHint : Hints)
	{
		if (InputHint.InputTag == InputTag)
		{
			return InputHint.GamepadButtonName;
		}
	}

	return FName();
}

FName UWitchForestInputHints::FindKeyboardButtonNameForInputTag(const FGameplayTag& InputTag) const
{
	for (const FWitchForestInputHint& InputHint : Hints)
	{
		if (InputHint.InputTag == InputTag)
		{
			return InputHint.GamepadButtonName;
		}
	}

	return FName();
}

UTexture2D* UWitchForestInputHints::FindGamepadButtonIconForInputTag(const FGameplayTag& InputTag) const
{
	for (const FWitchForestInputHint& InputHint : Hints)
	{
		if (InputHint.InputTag == InputTag)
		{
			return InputHint.GamepadButtonIcon;
		}
	}

	return nullptr;
}

UTexture2D* UWitchForestInputHints::FindKeyboardButtonIconForInputTag(const FGameplayTag& InputTag) const
{
	for (const FWitchForestInputHint& InputHint : Hints)
	{
		if (InputHint.InputTag == InputTag)
		{
			return InputHint.KeyboardButtonIcon;
		}
	}

	return nullptr;
}
