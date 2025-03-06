// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class mi_finder : ModuleRules
{
	public mi_finder(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
