// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class EffectBlendVolumeRuntime : ModuleRules
{
	public EffectBlendVolumeRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
                new string[]
                {
                "Core",
                "Engine",
                    // ... add other public dependencies that you statically link with here ...
                }
                );


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",

				// ... add private dependencies that you statically link with here ...	
			}
         );
    }
}
