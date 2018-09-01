using UnrealBuildTool;

public class HSVColorPluginRuntime : ModuleRules
{
	public HSVColorPluginRuntime(ReadOnlyTargetRules Target) : base(Target)
    {
		PublicDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"CoreUObject",
				"Engine",
				"Slate"
			}
		);
	}
}