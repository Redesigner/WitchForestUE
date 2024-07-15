// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class WitchForestGeneration : ModuleRules
{
	public WitchForestGeneration(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"WitchForestGame"
		});

		PrivateDependencyModuleNames.AddRange(new string[]{
        });
	}
}
