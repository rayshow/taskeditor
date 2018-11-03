/**

Copyright 2018 swd. All rights reserved.

*/

using UnrealBuildTool;

public class PSD2UMG : ModuleRules
{
	public PSD2UMG(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);

		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
					"Core", "CoreUObject", "Engine", "UnrealEd", "AssetTools", "Slate", "SlateCore", "RenderCore", "UMG", "Slate", "SlateCore", "UMGEditor", "AssetRegistry",
				// ... add other public dependencies that you statically link with here ...
			}
			);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
					"Core", "CoreUObject", "Engine", "UnrealEd", "AssetTools", "Slate", "SlateCore", "RenderCore", "UMG", "Slate", "SlateCore", "UMGEditor", "AssetRegistry",
				// ... add private dependencies that you statically link with here ...
			}
			);

		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);

		LoadLib(Target, "PSDParser");

		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
	}

	private string ThirdPartyPath
	{
		get { return System.IO.Path.GetFullPath(System.IO.Path.Combine(ModuleDirectory, "../../ThirdParty/")); }
	}

	public bool LoadLib(ReadOnlyTargetRules Target, string libName)
	{
		bool isLibrarySupported = false;

		if ((Target.Platform == UnrealTargetPlatform.Win64))
		{
			isLibrarySupported = true;

			string PlatformString = "x64";
			string LibrariesPath = System.IO.Path.Combine(ThirdPartyPath, libName, "Libraries");

			PublicAdditionalLibraries.Add(System.IO.Path.Combine(LibrariesPath, libName + "." + PlatformString + ".lib"));
		}

		if ((Target.Platform == UnrealTargetPlatform.Mac))
		{
			isLibrarySupported = true;

			string PlatformString = "Mac";
			string LibrariesPath = System.IO.Path.Combine(ThirdPartyPath, libName, "Libraries");

			PublicAdditionalLibraries.Add(System.IO.Path.Combine(LibrariesPath, "lib" + libName + "." + PlatformString + ".a"));
		}

		if (isLibrarySupported)
		{
			PrivateIncludePaths.Add(System.IO.Path.Combine(ThirdPartyPath, libName, "Includes"));
		}

		return isLibrarySupported;
	}
}
