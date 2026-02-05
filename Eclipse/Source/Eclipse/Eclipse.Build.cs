// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Eclipse : ModuleRules
{
	public Eclipse(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

			PublicDependencyModuleNames.AddRange(new string[] {
		"Core", 
		"CoreUObject",
		"Engine",                                                                                                                                                                                                        
		"InputCore",
		"EnhancedInput",
		"AIModule",
		"GameplayTasks",
		"NavigationSystem",
		"StateTreeModule",
		"GameplayStateTreeModule",
		"UMG",
		"Niagara"  // Niagara 모듈 추가!
	});

        PrivateDependencyModuleNames.AddRange(new string[] { 
			"GeometryCollectionEngine",
			"FieldSystemEngine",
			"ChaosSolverEngine"
		});

		PublicIncludePaths.AddRange(new string[] {
			"Eclipse",
			"Eclipse/Variant_Platforming",
			"Eclipse/Variant_Combat",
			"Eclipse/Variant_Combat/AI",
			"Eclipse/Variant_SideScrolling",
			"Eclipse/Variant_SideScrolling/Gameplay",
			"Eclipse/Variant_SideScrolling/AI",
			"Eclipse/Player",
			"Eclipse/Enemies",
			"Eclipse/GameFramework",
			"Eclipse/Gameplay"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}