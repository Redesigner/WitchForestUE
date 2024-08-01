// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class WitchForestUI : ModuleRules
{
	public WitchForestUI(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"GameplayTags",
			"AIModule",
			"CommonUI",
			"CommonInput",
			"UMG"
        });

		PrivateDependencyModuleNames.AddRange(new string[] {
            "WitchForestGame",
			"WitchForestAbility",
			"GameplayAbilities",
            "GameplayMessageRuntime"
        });
	}
}
