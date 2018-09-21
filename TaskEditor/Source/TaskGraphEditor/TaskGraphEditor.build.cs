// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TaskGraphEditor : ModuleRules
{
    public TaskGraphEditor(ReadOnlyTargetRules Target) : base(Target)
    {

        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[] {
                "TaskGraphEditor/Public",
                "TaskGraphEditor/Classes"
			}
            );


        PrivateIncludePaths.AddRange(
            new string[] {
                "TaskGraphEditor/Private",
			}
            );


        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", 
            }
        );

        //"ExcelOps",
        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Projects",
                "InputCore",
                "UnrealEd",
                "LevelEditor",
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "EditorStyle",
                "AssetTools",
                "GraphEditor",
                "ApplicationCore",
                "PropertyEditor",
                "ExcelOps",
            }
       );

        AddEngineThirdPartyPrivateStaticDependencies(Target,
            "ExcelOpsLibrary"
        );

        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
				
			}
        );
    }
}
