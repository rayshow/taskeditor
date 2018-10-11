// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class HeightFogVolumeRuntime : ModuleRules
{
	public HeightFogVolumeRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
 
	 
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "Engine",
                "Renderer",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
                "Renderer",
				// ... add private dependencies that you statically link with here ...	
			}
			);
 
	}
}
