// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class WitchForestAbility : ModuleRules
{
	public WitchForestAbility(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "GameplayAbilities", "GameplayTasks" });

		PrivateDependencyModuleNames.AddRange(new string[] { "GameplayTags" });
	}
}
