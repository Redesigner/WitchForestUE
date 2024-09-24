// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class WitchForestGame : ModuleRules
{
	public WitchForestGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"GameplayTags",
            "AIModule",
			"UMG"
		});

		PrivateDependencyModuleNames.AddRange(new string[]{
			"GameplayAbilities",
			"GameplayTasks",
			"NavigationSystem",
            "GameplayMessageRuntime",
			"WitchForestAbility"
        });
	}
}
