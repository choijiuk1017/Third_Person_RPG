// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Third_Person_RPG : ModuleRules
{
	public Third_Person_RPG(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core",
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput",
            "UMG",
			"Slate",        // UI 사용 시 필요
			"SlateCore"     // FSlateBrush 등 사용 시 반드시 필요
		});

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
