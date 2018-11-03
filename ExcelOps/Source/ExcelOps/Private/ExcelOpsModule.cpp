// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ExcelOpsModule.h"
#include "Core.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "Excel.h"

#define LOCTEXT_NAMESPACE "FExcelOpsModule"

void FExcelOpsModule::StartupModule()
{
	FString Str;
	

	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Get the base directory of this plugin
	FString BaseDir = IPluginManager::Get().FindPlugin("ExcelOps")->GetBaseDir();

	// Add on the relative location of the third party dll and load it
	FString LibraryPath;
#if PLATFORM_WINDOWS
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/ExcelOpsLibrary/Win64/Release/xlnt.dll"));
#endif 

	ExampleLibraryHandle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath) : nullptr;
	if (ExampleLibraryHandle)
	{
		/*Excel ex;
		ex.Value("A1",5);
		ex.Value("A2", L"大家好 data");
		ex.Value("B2", 4.5f);
		ex.Save(L"你好啊.xlsx");

		Excel ex2{L"测试example_.xlsx"};
		FString A2 = ex2.Value<FString>("A2");
		int a = 0;*/
	}
}

void FExcelOpsModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	// Free the dll handle
	FPlatformProcess::FreeDllHandle(ExampleLibraryHandle);
	ExampleLibraryHandle = nullptr;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FExcelOpsModule, ExcelOps)
