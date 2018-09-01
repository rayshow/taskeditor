using UnrealBuildTool;

public class HSVColorPluginEditor : ModuleRules
{
	public HSVColorPluginEditor(ReadOnlyTargetRules Target) : base(Target)
    {
		PublicDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"CoreUObject",
				"Engine",
				"Slate",
                "SlateCore",
                "AppFramework",
                "UnrealEd",
                "HSVColorPluginRuntime",
                "ContentBrowser"
            }
		);
		
		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"InputCore",
				"SlateCore",
				"PropertyEditor",
				"LevelEditor",
                "EditorStyle","UnrealEd",
                "Slate",  "MainFrame", "GraphEditor", "BlueprintGraph",
                "ContentBrowser",
                "PlacementMode",
            }
		);
	}
}