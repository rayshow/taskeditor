// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "StandaloneWindow.h"
#include "StandaloneWindowStyle.h"
#include "StandaloneWindowCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Editor/PropertyEditor/Public/PropertyEditorModule.h"
#include "Test1.h"
#include "IDetailsView.h"
#include"ModuleManager.h"

static const FName StandaloneWindowTabName("StandaloneWindow");

#define LOCTEXT_NAMESPACE "FStandaloneWindowModule"

void FStandaloneWindowModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FStandaloneWindowStyle::Initialize();
	FStandaloneWindowStyle::ReloadTextures();

	FStandaloneWindowCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FStandaloneWindowCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FStandaloneWindowModule::PluginButtonClicked),
		FCanExecuteAction());
		
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	
	{
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
		MenuExtender->AddMenuExtension("EditEdit", EExtensionHook::After, PluginCommands, FMenuExtensionDelegate::CreateRaw(this, &FStandaloneWindowModule::AddMenuExtension));

		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	}
	
	{
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FStandaloneWindowModule::AddToolbarExtension));
		
		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(StandaloneWindowTabName, FOnSpawnTab::CreateRaw(this, &FStandaloneWindowModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FStandaloneWindowTabTitle", "StandaloneWindow"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FStandaloneWindowModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FStandaloneWindowStyle::Shutdown();

	FStandaloneWindowCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(StandaloneWindowTabName);
}

TSharedRef<SDockTab> FStandaloneWindowModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
		FText::FromString(TEXT("FStandaloneWindowModule::OnSpawnPluginTab")),
		FText::FromString(TEXT("StandaloneWindow.cpp"))
		);

	
	auto UT = NewObject<UTest>();
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs(false, false, false, FDetailsViewArgs::HideNameArea);
	auto DetailsPanel = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	DetailsPanel->SetIsPropertyVisibleDelegate(FIsPropertyVisible::CreateLambda([](const FPropertyAndParent&) {return true; }));
	DetailsPanel->SetObject(UT);

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Put your tab content here!
			SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				DetailsPanel
			]
		];
}

void FStandaloneWindowModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->InvokeTab(StandaloneWindowTabName);
}

void FStandaloneWindowModule::AddMenuExtension(FMenuBuilder& Builder)
{
	Builder.AddMenuEntry(FStandaloneWindowCommands::Get().OpenPluginWindow);
}

void FStandaloneWindowModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
	Builder.AddToolBarButton(FStandaloneWindowCommands::Get().OpenPluginWindow);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FStandaloneWindowModule, StandaloneWindow)