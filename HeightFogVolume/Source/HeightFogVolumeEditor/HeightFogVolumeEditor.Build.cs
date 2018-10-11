// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class HeightFogVolumeEditor : ModuleRules
{
	public HeightFogVolumeEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateIncludePathModuleNames.AddRange(
            new string[] {
                "ContentBrowser",
            }
        );

        DynamicallyLoadedModuleNames.AddRange(
            new string[] {
               "ContentBrowser",
            }
        );

        PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "Engine",
                "HeightFogVolumeRuntime",
                //"ContentBrowser",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
                "PlacementMode",
                "UnrealEd",
				"HeightFogVolumeRuntime",
                //"ContentBrowser",
				// ... add private dependencies that you statically link with here ...	
			}
			);
 
	}
}
