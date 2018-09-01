#include "HSVColorPluginModule.h"
#include "HSVColorEditorCommand.h"
#include "EditorToolBarCommand.h"
#include "LevelEditor.h"
#include "ContentBrowserModule.h"
#include "ModuleManager.h"
#include "MainFrame.h"
#include "ScopedTransaction.h"
#include "FileHelpers.h"
#include "Runtime/Slate/Public/Widgets/Input/SNumericEntryBox.h"
#include "Runtime/Engine/Classes/Particles/ParticleModuleRequired.h"
#include "Runtime/Engine/Classes/Particles/Spawn/ParticleModuleSpawn.h"
#include "Runtime/Engine/Classes/Particles/ParticleLODLevel.h"
#include "Runtime/Engine/Classes/Particles/Color/ParticleModuleColor.h"
#include "Runtime/Engine/Classes/Particles/Color/ParticleModuleColorOverLife.h"
#include "Runtime/Engine/Classes/Particles/Color/ParticleModuleColorScaleOverLife.h"
#include "Editor/PlacementMode/Public/IPlacementModeModule.h"

#define LOCTEXT_NAMESPACE "HSVColorPlugin"
const static FName SPluginTabName("HSVAdjustingEditor");

void FHSVColorPluginModule::StartupModule()
{
	HSVOffset.A = 1;

	FHSVColorEditorCommand::Register();
	EditorToolBarCommand::Register();

	CommandList = MakeShareable(new FUICommandList);

	//get level module
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
	CommandList->Append(LevelEditorModule.GetGlobalLevelEditorActions());
	CommandList->MapAction(
		FHSVColorEditorCommand::Get().TestCommand,
		FExecuteAction::CreateStatic(&FHSVColorPluginModule::HandleTestCommandExcute),
		FCanExecuteAction::CreateStatic(&FHSVColorPluginModule::HandleTestCommandCanExcute)
	);
	auto& module = IPlacementModeModule::Get();

	//create a menu button 
	TSharedRef<FExtender> MenuExtender(new FExtender());
	MenuExtender->AddMenuExtension(
		TEXT("EditMain"),
		EExtensionHook::After,
		CommandList.ToSharedRef(),
		FMenuExtensionDelegate::CreateLambda([this](FMenuBuilder& MenuBuilder) {
			MenuBuilder.AddMenuEntry(
				LOCTEXT("HSVEdtor", "Effect HSV Adjuster"),
				LOCTEXT("DemoToolsTooltip", "List of tools"),
				FSlateIcon(),
				FExecuteAction::CreateRaw(this, &FHSVColorPluginModule::OpenHSVColorWindow));
	}));
	LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);

	//create a tool bar button
	TSharedRef<FExtender> ToolbarExtender(new FExtender());
	ToolbarExtender->AddToolBarExtension(
		TEXT("Game"),
		EExtensionHook::After,
		CommandList.ToSharedRef(),
		FToolBarExtensionDelegate::CreateLambda([](FToolBarBuilder& ToolbarBuilder) {
			ToolbarBuilder.AddToolBarButton(FHSVColorEditorCommand::Get().TestCommand);
	}));
	LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);

	//register plugin tab
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(SPluginTabName,
		FOnSpawnTab::CreateRaw(this, &FHSVColorPluginModule::SpawnTabPanel));
	TabWindowOpened = false;
}

void FHSVColorPluginModule::ShutdownModule()
{
	//unregister tab
	FGlobalTabmanager::Get()->UnregisterTabSpawner(SPluginTabName);
	FHSVColorEditorCommand::Unregister();
	EditorToolBarCommand::Unregister();
	TabWindowOpened = false;
}

//open window
void FHSVColorPluginModule::OpenHSVColorWindow()
{
	if (TabWindowOpened) return;
	TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout("HSVColorPlugin_Layout")
		->AddArea
		(
			FTabManager::NewArea(720, 600)->Split(
				FTabManager::NewStack()->AddTab(SPluginTabName, ETabState::OpenedTab) )
		);
	
	FGlobalTabmanager::Get()->RestoreFrom(Layout, NULL);

}

FReply FHSVColorPluginModule::OnFromFileButtonClicked() {

	ClearPanelContent();

	//get selected files
	TArray<FAssetData> SelectedFiles;
	GEditor->GetContentBrowserSelections(SelectedFiles);

	//loop over files
	int FileIndex = 0;
	for (auto& AssetData : SelectedFiles) {
		++FileIndex;

		//asset is particle effect asset file
		if (auto ParticleSystem = Cast<UParticleSystem>(AssetData.GetAsset())) {
			//analysis color infos
			AnalysisParticleSystem(ParticleSystem,nullptr);
		}
		if (auto Blueprint = Cast<UBlueprint>(AssetData.GetAsset())) {
			// get all ParticleSystemComponent from this blueprint
			const TArray<USCS_Node*>& Nodes = Blueprint->SimpleConstructionScript->GetAllNodes();
			for (USCS_Node* Node : Nodes)
			{
					UBlueprintGeneratedClass* GeneratedClass = Cast<UBlueprintGeneratedClass>(Blueprint->GeneratedClass);
					if (GeneratedClass)
					{
						UActorComponent* Component = Node->GetActualComponentTemplate(GeneratedClass);
						if (Component ) {
							if (auto ParticleSystemComp = Cast<UParticleSystemComponent>(Component)) {
								AnalysisParticleSystem(ParticleSystemComp->Template, Blueprint);
							}
						}
					}
			}
		}
	}
	//ContentBrower.Get().
	return FReply::Handled();
}

// analysis color infos
FReply FHSVColorPluginModule::OnAnalysisButtonClicked() {
	
	ClearPanelContent();

	for (FSelectionIterator It(GEditor->GetSelectedActorIterator()); It; ++It)
	{
		if (AActor* Actor = Cast<AActor>(*It))
		{
			auto ParticleSystemComps = Actor->GetComponentsByClass(UParticleSystemComponent::StaticClass());
			for (auto Comp : ParticleSystemComps)
			{
				if (auto ParticleSystemComp = Cast<UParticleSystemComponent>(Comp)) 
				{
					AnalysisParticleSystem(ParticleSystemComp->Template, Actor);
				}
			}
		}	
	}
	return FReply::Handled();
}

//clear content
FReply FHSVColorPluginModule::OnClearButtonClicked() {
	ClearPanelContent();
	return FReply::Handled();
}

//recover old color
FReply FHSVColorPluginModule::OnResetButtonClicked() {
	ColorPanel->ApplyOldColor();
	ResetSlider();
	ResetParticleSystem();
	return FReply::Handled();
}


void FHSVColorPluginModule::ClearPanelContent() {
	ColorPanel->CleanGroup();
	ObjectsToSave.Empty();
	ResetSlider();
}

//reset slider position and value
void FHSVColorPluginModule::ResetSlider() {
	HSVOffset = FLinearColor{0,1,1,1};
	ColorPanel->ApplyHSVAdjusting(HSVOffset);
}


void FHSVColorPluginModule::ResetParticleSystem() {
	for (auto Obj : ObjectsToSave) {
		Cast<UParticleSystem>(Obj)->BuildEmitters();
	}
}

void FHSVColorPluginModule::ApplyColorChange() {
	ColorPanel->ApplyHSVAdjusting(HSVOffset);
	ColorPanel->ApplyNewColor();
	ResetParticleSystem();
}


//save changes
FReply FHSVColorPluginModule::OnSaveAndExit() {
	TArray<UPackage*> PackagesToSave;


	for (auto Object : ObjectsToSave)
	{
		check((Object != nullptr) && Object->IsAsset());
		PackagesToSave.Add(Object->GetOutermost());
	}

	// check asset dirty and not prompt to save
	FEditorFileUtils::PromptForCheckoutAndSave(PackagesToSave, false, false);
	return FReply::Handled();
}


// Build Content
void FHSVColorPluginModule::AnalysisParticleSystem(UParticleSystem* ParticleSystem, UObject* Base) {
	if (!ParticleSystem) return;

	//record this particle system
	ObjectsToSave.Add(ParticleSystem);

	FString Path;
	Path.Reserve(64);
	if (Base) {
		Path.Append(Base->GetName()).Append(".").Append(ParticleSystem->GetName()).Append(".");
	}
	else {
		Path.Append(ParticleSystem->GetName()).Append(".");
	}
	

	ParticleSystem->EditorLODSetting = 0;
	ParticleSystem->SetupLODValidity();
	// Support undo/redo
	ParticleSystem->SetFlags(RF_Transactional);

	for (auto Emitter : ParticleSystem->Emitters) {
		Emitter->SetFlags(RF_Transactional);
		int level = 0;
		for (auto LodLevel : Emitter->LODLevels)
		{
			++level;
			LodLevel->SetFlags(RF_Transactional);
			LodLevel->RequiredModule->SetTransactionFlag();
			LodLevel->SpawnModule->SetTransactionFlag();
			
			for (auto Module : LodLevel->Modules) {
				
				Module->SetTransactionFlag();
				//this module is color relative module
				if (auto ColorModule = Cast<UParticleModuleColorBase>(Module)) {

					UDistributionVector* VectorCurves{ nullptr };
					//is initial color module, get it's initial color 
					if (auto InitColorModules = Cast<UParticleModuleColor>(Module)) {
						VectorCurves = InitColorModules->StartColor.Distribution;
					}
					// is ColorOverLifeTime module, get it's begin and end colors
					else if (auto ColorOverLifeTime = Cast<UParticleModuleColorOverLife>(Module)) {
						VectorCurves = ColorOverLifeTime->ColorOverLife.Distribution;
					}
					// is ColorScaleOverLife module, get it's ScaleOverLife
					else if (auto ScaleOverLifeTime = Cast<UParticleModuleColorScaleOverLife>(Module)) {
						VectorCurves = ScaleOverLifeTime->ColorScaleOverLife.Distribution;

					}
					// unreachable
					else { check(false); }

					if (VectorCurves) {
						auto group = SNew(SColorCmpGroup);
						group->SetPath(FText::FromString(
							Path + Emitter->GetName()
							+ FString::Printf(TEXT(".Lod%d."), level)
							+ Module->GetName()
						));

						for (int j = 0; j < VectorCurves->GetNumKeys(); ++j)
						{
							FLinearColor Start{ 1,0,0,1 };
							FLinearColor End{ 0,1,0,1 };

							if (VectorCurves->GetNumSubCurves() == 3)
							{
								Start.R = VectorCurves->GetKeyOut(0, j);
								Start.G = VectorCurves->GetKeyOut(1, j);
								Start.B = VectorCurves->GetKeyOut(2, j);
								auto row = SNew(SColorCmpRow).OldColor(Start).KeyColorOwner(VectorCurves).IsBegin(true).KeyIndex(j);
								group->AddNewRow(row);
							}
							else if (VectorCurves->GetNumSubCurves() == 6)
							{
								Start.R = VectorCurves->GetKeyOut(0, j);
								Start.G = VectorCurves->GetKeyOut(1, j);
								Start.B = VectorCurves->GetKeyOut(2, j);
								End.R = VectorCurves->GetKeyOut(3, j);
								End.G = VectorCurves->GetKeyOut(4, j);
								End.B = VectorCurves->GetKeyOut(5, j);

								auto rowS = SNew(SColorCmpRow).OldColor(Start).KeyColorOwner(VectorCurves).IsBegin(true).KeyIndex(j);
								auto rowE = SNew(SColorCmpRow).OldColor(End).KeyColorOwner(VectorCurves).IsBegin(false).KeyIndex(j);
								group->AddNewRow(rowS);
								group->AddNewRow(rowE);
							}
						}
						ColorPanel->AddGroup(group);
					}
				}
			}
		}
	}
}

//setup main config panel
TSharedRef<SDockTab> FHSVColorPluginModule::SpawnTabPanel(const FSpawnTabArgs& Args)
{
	TSharedRef<SDockTab> HSVColorPluginTab =
		SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		.Label(LOCTEXT("HSVColorPluginTab", "HSV Color Plugin Tab"))
		.ToolTipText(LOCTEXT("HSVColorPluginTabToolTip", "use for particle HSV adjusting."))
		.OnTabClosed_Lambda([&](const TSharedRef<SDockTab>& tab) { 
			TabWindowOpened = false;	
		}) ;
	
	TabWindowOpened = true;

	HueValueText = SNew(SNumericEntryBox<float>)
		.AllowSpin(true)
		.MinSliderValue(-180)
		.MaxSliderValue(180)
		.Value_Lambda([&]() { return HSVOffset.R; })
		.OnValueCommitted_Lambda([&](float R, ETextCommit::Type) { HSVOffset.R = R; ApplyColorChange(); })
		.OnValueChanged_Lambda([&](float R) { HSVOffset.R = R; ApplyColorChange();  });

	SaturationValueText = SNew(SNumericEntryBox<float>)
		.AllowSpin(true)
		.MinSliderValue(0)
		.MaxSliderValue(1)
		.Value_Lambda([&]() { return HSVOffset.G; })
		.OnValueCommitted_Lambda([&](float G, ETextCommit::Type) { HSVOffset.G = G; ApplyColorChange();  })
		.OnValueChanged_Lambda([&](float G) {  HSVOffset.G = G; ApplyColorChange(); });

	ValueValueText = SNew(SNumericEntryBox<float>)
		.AllowSpin(true)
		.MinSliderValue(0)
		.MaxSliderValue(2)
		.Value_Lambda([&]() { return HSVOffset.B; })
		.OnValueCommitted_Lambda([&](float B, ETextCommit::Type) { HSVOffset.B = B; ApplyColorChange();  })
		.OnValueChanged_Lambda([&](float B) { HSVOffset.B = B; ApplyColorChange();  });

	ColorPanel = SNew(SColorAnalysisPanel);
	HSVOffset = FLinearColor{ 0,1,1,1 };

	HSVColorPluginTab->SetContent(
		SNew(SVerticalBox)
		+ SVerticalBox::Slot().AutoHeight()
		[
			//first line of buttons
			SNew(SHorizontalBox) + SHorizontalBox::Slot().Padding(50, 5)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot().FillWidth(1)
				[
					//bind analysis button click event
					SNew(SButton).Text(LOCTEXT("B1", "Analysis Asset")).HAlign(HAlign_Center)
					.OnClicked_Raw(this, &FHSVColorPluginModule::OnFromFileButtonClicked)
				]
				+ SHorizontalBox::Slot().FillWidth(1)
				[
					//bind analysis from world level select actor button click event
					SNew(SButton).Text(LOCTEXT("B5", "Analysis Actor")).HAlign(HAlign_Center)
					.OnClicked_Raw(this, &FHSVColorPluginModule::OnAnalysisButtonClicked)
				]
				+ SHorizontalBox::Slot().FillWidth(1)
				[
					//bind clear button click event
					SNew(SButton).Text(LOCTEXT("B2", "Clear")).HAlign(HAlign_Center)
					.OnClicked_Raw(this, &FHSVColorPluginModule::OnClearButtonClicked)
				]
				+ SHorizontalBox::Slot().FillWidth(1)
				[
					//bind use new Color button click event
					SNew(SButton).Text(LOCTEXT("B3", "Reset")).HAlign(HAlign_Center)
					.OnClicked_Raw(this, &FHSVColorPluginModule::OnResetButtonClicked)
				]
			]
		]
		//second line of hue slider
		+ SVerticalBox::Slot().AutoHeight().Padding(30, 5)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().FillWidth(1)
			[
				SNew(STextBlock).Text(LOCTEXT("T1", "H offset: "))
			]
			+ SHorizontalBox::Slot().FillWidth(6)
			[
				HueValueText.ToSharedRef()
			]
		]
		+ SVerticalBox::Slot().AutoHeight().Padding(30, 5)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().FillWidth(1)
			[
				SNew(STextBlock).Text(LOCTEXT("T2", "S multiply : "))
			]
			+ SHorizontalBox::Slot().FillWidth(6)
			[
				SaturationValueText.ToSharedRef()
			]
		]
		+ SVerticalBox::Slot().AutoHeight().Padding(30, 5)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().FillWidth(1)
			[
				SNew(STextBlock).Text(LOCTEXT("T3", "V multiply : "))
			]
			+ SHorizontalBox::Slot().FillWidth(6)
			[
				ValueValueText.ToSharedRef()
			]
		]
		+ SVerticalBox::Slot()[ColorPanel.ToSharedRef()]
		+ SVerticalBox::Slot().AutoHeight()
			[	
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot().Padding(0,10).HAlign(HAlign_Center)
				[
					SNew(SButton).ContentPadding(FMargin(15,10))
					.Text(LOCTEXT("B6", "Save")).HAlign(HAlign_Center)
					.OnClicked_Raw(this, &FHSVColorPluginModule::OnSaveAndExit)
				]
			]
	);

	return HSVColorPluginTab;
}



void FHSVColorPluginModule::HandleTestCommandExcute()
{
	FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("Test Command Excuted!!!"), TEXT("TestCommand"));
}

bool FHSVColorPluginModule::HandleTestCommandCanExcute()
{
	return true;
}

IMPLEMENT_MODULE(FHSVColorPluginModule, HSVColorPlugin);

#undef LOCTEXT_NAMESPACE