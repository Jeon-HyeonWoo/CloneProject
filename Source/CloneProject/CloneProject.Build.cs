// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CloneProject : ModuleRules
{
	public CloneProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"GameplayTags",	//Game Ability System //PlugIn
			"ModularGameplay",	//Game Features // PlugIn
			"GameFeatures", //
			"ModularGameplayActors",
			"InputCore",
			"EnhancedInput",
			"CommonUser" //Custom Moudule for Move to Map
		});

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
