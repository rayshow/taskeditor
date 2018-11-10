// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "PropertyEditorToolkit.h"
#include "Engine/Blueprint.h"
#include "Editor.h"
#include "EditorStyleSet.h"
#include "PropertyEditorModule.h"
#include "Modules/ModuleManager.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "IPropertyTableColumn.h"
#include "IPropertyTreeRow.h"
#include "IPropertyTableRow.h"

#include "Widgets/Docking/SDockTab.h"


//jarviezhang begin
#include "Runtime/Slate/Public/Framework/MultiBox/MultiBoxBuilder.h"
#include "Editor/ContentBrowser/Private/SFilterList.h"
#include "FrontendFilters.h"
#include "Editor/ContentBrowser/Public/ContentBrowserDelegates.h"
#include "Runtime/Slate/Public/Widgets/Input/SComboButton.h"

#include "Runtime/Engine/Classes/Engine/SkeletalMesh.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Runtime/Slate/Public/Widgets/Input/SCheckBox.h"
//jarviezhang end

#define LOCTEXT_NAMESPACE "PropertyEditorToolkit"

const FName FPropertyEditorToolkit::ToolkitFName( TEXT( "PropertyEditor" ) );
const FName FPropertyEditorToolkit::ApplicationId( TEXT( "PropertyEditorToolkitApp" ) );
const FName FPropertyEditorToolkit::TreeTabId( TEXT( "PropertyEditorToolkit_PropertyTree" ) );
const FName FPropertyEditorToolkit::GridTabId( TEXT( "PropertyEditorToolkit_PropertyTable" ) );
const FName FPropertyEditorToolkit::TreePinAsColumnHeaderId( TEXT( "PropertyEditorToolkit_PinAsColumnHeader" ) );
//jarviezhang begin
const FName FPropertyEditorToolkit::AdditionalSettingsTabId(TEXT("PropertyEditorToolkit_AdditionalSettings"));
//jarviezhang end

void FPropertyEditorToolkit::RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_PropertyEditorToolkit", "Property Editor"));
	InTabManager->RegisterTabSpawner( GridTabId, FOnSpawnTab::CreateSP(this, &FPropertyEditorToolkit::SpawnTab_PropertyTable) )
		.SetDisplayName( LOCTEXT("PropertyTableTab", "Grid") )
		.SetGroup( WorkspaceMenuCategory.ToSharedRef() )
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Viewports"));

	InTabManager->RegisterTabSpawner( TreeTabId, FOnSpawnTab::CreateSP(this, &FPropertyEditorToolkit::SpawnTab_PropertyTree) )
		.SetDisplayName( LOCTEXT("PropertiesTab", "Details") )
		.SetGroup( WorkspaceMenuCategory.ToSharedRef() )
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "PropertyEditor.Grid.TabIcon"));

	//jarviezhang begin

	//Register toolbar
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(AdditionalSettingsTabId, FOnSpawnTab::CreateSP(this, &FPropertyEditorToolkit::SpawnTab_AdditionalSettings))
		.SetDisplayName(LOCTEXT("PropertyAdditionalSettingsTab", "Additional Settings"))
		.SetGroup(WorkspaceMenuCategory.ToSharedRef())
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "PropertyEditor.Grid.TabIcon"));

	//jarviezhang end
}

void FPropertyEditorToolkit::UnregisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	InTabManager->UnregisterTabSpawner( GridTabId );
	InTabManager->UnregisterTabSpawner( TreeTabId );
	//jarviezhang begin
	InTabManager->UnregisterTabSpawner(AdditionalSettingsTabId);
	//jarviezhang end
}


FPropertyEditorToolkit::FPropertyEditorToolkit()
	: PropertyTree()
	, PropertyTable()
	, PathToRoot()
{
	PinSequence.AddCurve( 0, 1.0f, ECurveEaseFunction::QuadIn );
}


TSharedPtr<FPropertyEditorToolkit> FPropertyEditorToolkit::FindExistingEditor( UObject* Object )
{
	// Find any existing property editor instances for this asset
	const TArray<IAssetEditorInstance*> Editors = FAssetEditorManager::Get().FindEditorsForAsset( Object );

	IAssetEditorInstance* const * ExistingInstance = Editors.FindByPredicate( [&]( IAssetEditorInstance* Editor ){
		return Editor->GetEditorName() == ToolkitFName;
	} );

	if( ExistingInstance )
	{
		auto* PropertyEditor = static_cast<FPropertyEditorToolkit*>( *ExistingInstance );
		return PropertyEditor->SharedThis( PropertyEditor );
	}

	return nullptr;
}

TSharedRef<FPropertyEditorToolkit> FPropertyEditorToolkit::CreateEditor( const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UObject* ObjectToEdit )
{
	auto ExistingEditor = FindExistingEditor( ObjectToEdit );
	if( ExistingEditor.IsValid() )
	{
		ExistingEditor->FocusWindow();
		return ExistingEditor.ToSharedRef();
	}

	TSharedRef< FPropertyEditorToolkit > NewEditor( new FPropertyEditorToolkit() );

	TArray<UObject*> ObjectsToEdit;
	ObjectsToEdit.Add( ObjectToEdit );
	NewEditor->Initialize( Mode, InitToolkitHost, ObjectsToEdit );

	return NewEditor;
}


TSharedRef<FPropertyEditorToolkit> FPropertyEditorToolkit::CreateEditor( const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, const TArray<UObject*>& ObjectsToEdit )
{
	if( ObjectsToEdit.Num() == 1 )
	{
		auto ExistingEditor = FindExistingEditor( ObjectsToEdit[0] );
		if( ExistingEditor.IsValid() )
		{
			ExistingEditor->FocusWindow();
			return ExistingEditor.ToSharedRef();
		}
	}

	TSharedRef< FPropertyEditorToolkit > NewEditor( new FPropertyEditorToolkit() );
	NewEditor->Initialize( Mode, InitToolkitHost, ObjectsToEdit );

	return NewEditor;
}


void FPropertyEditorToolkit::Initialize( const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, const TArray<UObject*>& ObjectsToEdit )
{
	TArray< UObject* > AdjustedObjectsToEdit;
	for( auto ObjectIter = ObjectsToEdit.CreateConstIterator(); ObjectIter; ++ObjectIter )
	{
		//@todo Remove this and instead extend the blueprints Edit Defaults editor to use a Property Table as well [12/6/2012 Justin.Sargent]
		UObject* Object = *ObjectIter;
		if ( Object->IsA( UBlueprint::StaticClass() ) )
		{
			UBlueprint* Blueprint = Cast<UBlueprint>( Object );

			// Make sure that the generated class is valid, in case the super has been removed, and this class can't be loaded.
			if( ensureMsgf(Blueprint->GeneratedClass != NULL, TEXT("Blueprint %s has no generated class"), *Blueprint->GetName()) )
			{
				AdjustedObjectsToEdit.Add( Blueprint->GeneratedClass->GetDefaultObject() );
			}
		}
		else
		{
			AdjustedObjectsToEdit.Add( Object );
		}
	}

	if(AdjustedObjectsToEdit.Num() > 0)
	{

		CreatePropertyTree();
		CreatePropertyTable();

		PropertyTable->SetObjects(AdjustedObjectsToEdit);
		TableColumnsChanged();

		//jarviezhang begin

		TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("Standalone_PropertyEditorToolkit_Layout")
			->AddArea
			(
				FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
				->Split
				(
					FTabManager::NewStack()
					//->SetSizeCoefficient(0.8f)
					->AddTab(GetToolbarTabId(), ETabState::OpenedTab)
				)
				->Split
				(
					FTabManager::NewPrimaryArea()->SetOrientation(Orient_Horizontal)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.8f)
						->AddTab(GridTabId, ETabState::OpenedTab)
					)
					->Split
					(
						FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
						->SetSizeCoefficient(0.2f)
						->Split
						(
							FTabManager::NewStack()
							->SetSizeCoefficient(0.7f)
							->AddTab(TreeTabId, ETabState::OpenedTab)
						)
						->Split
						(
							FTabManager::NewStack()
							->SetSizeCoefficient(0.3f)
							->AddTab(AdditionalSettingsTabId, ETabState::OpenedTab)
						)
					)
				)

			);

		//jarviezhang end

		const bool bCreateDefaultStandaloneMenu = true;
		//jarviezhang begin
		const bool bCreateDefaultToolbar = true;
		//jarviezhang end

		FAssetEditorToolkit::InitAssetEditor(Mode, InitToolkitHost, ApplicationId, StandaloneDefaultLayout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, AdjustedObjectsToEdit);

		TArray< TWeakObjectPtr<UObject> > AdjustedObjectsToEditWeak;
		for (auto ObjectIter = AdjustedObjectsToEdit.CreateConstIterator(); ObjectIter; ++ObjectIter)
		{
			AdjustedObjectsToEditWeak.Add(*ObjectIter);
		}
		PropertyTree->SetObjectArray(AdjustedObjectsToEditWeak);

		PinColor = FSlateColor(FLinearColor(1, 1, 1, 0));
		GEditor->GetTimerManager()->SetTimer(TimerHandle_TickPinColor, FTimerDelegate::CreateSP(this, &FPropertyEditorToolkit::TickPinColorAndOpacity), 0.1f, true);
	}

	//jarviezhang begin
	CurLODIndex = 0;
	//Generate filter
	FrontendFilters = MakeShareable(new FAssetFilterCollectionType());
	FilterListPtr = SNew(SFilterList)
		.FrontendFilters(FrontendFilters)
		.OnFilterChanged(this, &FPropertyEditorToolkit::OnFilterChanged);

	//Extend the toolbar
	TSharedPtr<FExtender> ToolBarExtender = MakeShareable(new FExtender);
	ToolBarExtender->AddToolBarExtension("Asset", EExtensionHook::After, NULL, FToolBarExtensionDelegate::CreateRaw(this, &FPropertyEditorToolkit::AddFilterButton));
	AddToolbarExtender(ToolBarExtender);
	RegenerateMenusAndToolbars();

	//jarviezhang end
}

//jarviezhang begin
void FPropertyEditorToolkit::AddFilterButton(FToolBarBuilder& Builder)
{
	TSharedRef<SComboButton> FilterButton = SNew(SComboButton)
		.ComboButtonStyle(FEditorStyle::Get(), "GenericFilters.ComboButtonStyle")
		.ForegroundColor(FLinearColor::White)
		.ContentPadding(0)
		.ToolTipText(LOCTEXT("PropertyEditorToolkit", "Filters"))
		.OnGetMenuContent(this, &FPropertyEditorToolkit::MakeAddFilterMenu)
		.HasDownArrow(true)
		.ContentPadding(FMargin(1, 0))
		.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("ContentBrowserFiltersCombo")))
		.ButtonContent()
		[
			SNew(STextBlock)
			.Text(LOCTEXT("PropertyEditorToolkit", "Filters"))
		]
	;
	Builder.AddWidget(FilterButton);

}


void FPropertyEditorToolkit::HandleLODEntryBoxChanged(int32 LODIndex)
{
	CurLODIndex = LODIndex;
}

TOptional<int32> FPropertyEditorToolkit::HandleLODEntryBoxValue() const
{
	return CurLODIndex;
}

TSharedRef<SWidget> FPropertyEditorToolkit::OnGenerateLodMenuForLodPicker()
{
	FMenuBuilder MenuBuilder(true, NULL);

	//FText AutoLodText = FText::FromString((TEXT("LOD Auto")));
	//FUIAction AutoLodAction(FExecuteAction::CreateSP(this, &FLevelOfDetailSettingsLayout::OnSelectedLODChanged, 0));
	//MenuBuilder.AddMenuEntry(AutoLodText, LOCTEXT("OnGenerateLodMenuForLodPicker_Auto_ToolTip", "LOD0 is edit when selecting Auto LOD"), FSlateIcon(), AutoLodAction);
	// Add a menu item for each texture.  Clicking on the texture will display it in the content browser
	for (int32 AllLodIndex = 0; AllLodIndex < MAX_STATIC_MESH_LODS ; ++AllLodIndex)
	{
		FText LODLevelString = FText::FromString((TEXT("LOD ") + FString::FromInt(AllLodIndex)));
		FUIAction Action(FExecuteAction::CreateSP(this, &FPropertyEditorToolkit::HandleLODEntryBoxChanged, AllLodIndex));
		MenuBuilder.AddMenuEntry(LODLevelString, FText::GetEmpty(), FSlateIcon(), Action);
	}

	return MenuBuilder.MakeWidget();
}

TOptional<float> FPropertyEditorToolkit::HandleScreenSizeEntryBoxValue() const
{
	TOptional<float> ResultScreenSize;
	ResultScreenSize.Reset();

	TArray<TWeakObjectPtr<UObject>> SelectedObjects;
	PropertyTable->GetSelectedTableObjects(SelectedObjects);
	if (SelectedObjects.Num() == 1)
	{
		UObject* Obj = SelectedObjects[0].Get();
		if (Obj->IsA(UStaticMesh::StaticClass()))
		{
			UStaticMesh* StaticMesh = Cast<UStaticMesh>(Obj);
			if (StaticMesh->SourceModels.IsValidIndex(CurLODIndex) && !StaticMesh->bAutoComputeLODScreenSize)
			{
				ResultScreenSize = StaticMesh->SourceModels[CurLODIndex].ScreenSize.Default;
			}
			else
			{
				ResultScreenSize.Reset();
			}
		}
		else if (Obj->IsA(USkeletalMesh::StaticClass()))
		{
			USkeletalMesh* SkeletalMesh = Cast<USkeletalMesh>(Obj);
			if (SkeletalMesh->IsValidLODIndex(CurLODIndex))
			{
				ResultScreenSize = SkeletalMesh->GetLODInfo(CurLODIndex)->ScreenSize.Default;
			}
			else
			{
				ResultScreenSize.Reset();
			}
		}
	}

	return ResultScreenSize;
}

void FPropertyEditorToolkit::HandleScreenSizeCommit(const float Value, ETextCommit::Type CommitType)
{
	float ScreenSize = FMath::Clamp(Value, 0.0f, 1.0f);

	GEditor->BeginTransaction(FText::FromString(FString::Printf(TEXT("Set Screensize:%.2f for LOD%d"), ScreenSize, CurLODIndex)));
	TArray< TWeakObjectPtr< UObject > > SelectedObjects;
	PropertyTable->GetSelectedTableObjects(SelectedObjects);
	for (int32 I = 0; I < SelectedObjects.Num(); I++)
	{
		UObject* Object = SelectedObjects[I].Get();
		Object->Modify();
		if (Object->IsA(UStaticMesh::StaticClass()))
		{
			//Object->Modify();
			SetStaticMeshLODScreenSize(Object, ScreenSize);
		}
		else if (Object->IsA(USkeletalMesh::StaticClass()))
		{
			//Object->Modify();
			SetSkeletalMeshLODScreenSize(Object, ScreenSize);
		}
	}
	GEditor->EndTransaction();

	return;
}


TOptional<float> FPropertyEditorToolkit::HandleTrianglePercentEntryBoxValue() const
{
	TOptional<float> TrianglePercent;
	TrianglePercent.Reset();

	TArray<TWeakObjectPtr<UObject>> SelectedObjects;
	PropertyTable->GetSelectedTableObjects(SelectedObjects);
	if (SelectedObjects.Num() == 1)
	{
		UObject* Obj = SelectedObjects[0].Get();
		if (Obj->IsA(UStaticMesh::StaticClass()))
		{
			UStaticMesh* StaticMesh = Cast<UStaticMesh>(Obj);
			if (StaticMesh->SourceModels.IsValidIndex(CurLODIndex))
			{
				TrianglePercent = StaticMesh->SourceModels[CurLODIndex].ReductionSettings.PercentTriangles;
			}
			else
			{
				TrianglePercent.Reset();
			}
		}
	}

	return TrianglePercent;
}

void FPropertyEditorToolkit::HandleTrianglePercentCommit(const float Value, ETextCommit::Type CommitType)
{
	float TrianglePercent = FMath::Clamp(Value, 0.0f, 1.0f);

	GEditor->BeginTransaction(FText::FromString(FString::Printf(TEXT("Set Percent Triangle :%.2f for LOD%d"), TrianglePercent, CurLODIndex)));
	TArray< TWeakObjectPtr< UObject > > SelectedObjects;
	PropertyTable->GetSelectedTableObjects(SelectedObjects);
	for (int32 I = 0; I < SelectedObjects.Num(); I++)
	{
		UObject* Obj = SelectedObjects[I].Get();
		Obj->Modify();

		if (Obj->IsA(UStaticMesh::StaticClass()))
		{
			UStaticMesh* StaticMesh = Cast<UStaticMesh>(Obj);
			if (StaticMesh->SourceModels.IsValidIndex(CurLODIndex))
			{
				StaticMesh->SourceModels[CurLODIndex].ReductionSettings.PercentTriangles = TrianglePercent;
			}
		}
	}
	GEditor->EndTransaction();

	return;
}


void FPropertyEditorToolkit::SetStaticMeshLODScreenSize(UObject* Obj, float ScreenSize)
{
	UStaticMesh* StaticMesh = Cast<UStaticMesh>(Obj);

	if (StaticMesh != nullptr && !StaticMesh->bAutoComputeLODScreenSize)
	{
		FPerPlatformFloat LODScreenSizes[MAX_STATIC_MESH_LODS];
		// First propagate any changes from the source models to our local scratch.
		for (int32 i = 0; i < StaticMesh->SourceModels.Num(); ++i)
		{
			LODScreenSizes[i] = StaticMesh->SourceModels[i].ScreenSize;
		}

		// Update Display factors for further LODs
		const float MinimumDifferenceInScreenSize = KINDA_SMALL_NUMBER;

		LODScreenSizes[CurLODIndex].Default = ScreenSize;

		// Make sure we aren't trying to overlap or have more than one LOD for a value
		for (int32 i = 1; i < MAX_STATIC_MESH_LODS; ++i)
		{
			float MaxValue = FMath::Max(LODScreenSizes[i - 1].Default - MinimumDifferenceInScreenSize, 0.0f);
			LODScreenSizes[i].Default = FMath::Min(LODScreenSizes[i].Default, MaxValue);
		}


		// Push changes immediately.
		StaticMesh->Modify();
		for (int32 i = 0; i < MAX_STATIC_MESH_LODS; ++i)
		{
			if (StaticMesh->SourceModels.IsValidIndex(i))
			{
				StaticMesh->SourceModels[i].ScreenSize = LODScreenSizes[i];
			}
			if (StaticMesh->RenderData
				&& StaticMesh->RenderData->LODResources.IsValidIndex(i))
			{
				StaticMesh->RenderData->ScreenSize[i] = LODScreenSizes[i];
			}
		}

	}
}

void FPropertyEditorToolkit::SetSkeletalMeshLODScreenSize(UObject* Obj, float ScreenSize)
{
	USkeletalMesh* SkeletalMesh = Cast<USkeletalMesh>(Obj);
	if (SkeletalMesh != nullptr)
	{
		FPerPlatformFloat LODScreenSizes[MAX_STATIC_MESH_LODS];
		TArray < FSkeletalMeshLODInfo >& LODInfoArray = SkeletalMesh->GetLODInfoArray();
		// First propagate any changes from the source models to our local scratch.
		for (int32 i = 0; i < LODInfoArray.Num(); ++i)
		{
			LODScreenSizes[i] = LODInfoArray[i].ScreenSize;
		}

		// Update Display factors for further LODs
		const float MinimumDifferenceInScreenSize = KINDA_SMALL_NUMBER;

		LODScreenSizes[CurLODIndex].Default = ScreenSize;

		// Make sure we aren't trying to overlap or have more than one LOD for a value
		for (int32 i = 1; i < MAX_STATIC_MESH_LODS; ++i)
		{
			float MaxValue = FMath::Max(LODScreenSizes[i - 1].Default - MinimumDifferenceInScreenSize, 0.0f);
			LODScreenSizes[i].Default = FMath::Min(LODScreenSizes[i].Default, MaxValue);
		}

		SkeletalMesh->Modify();
		// Push changes immediately.
		for (int32 i = 0; i < MAX_STATIC_MESH_LODS; ++i)
		{
			if (LODInfoArray.IsValidIndex(i))
			{
				LODInfoArray[i].ScreenSize = LODScreenSizes[i];
			}
		}

	}
}


ECheckBoxState FPropertyEditorToolkit::HandleAutoScreensizeCheckBoxIsChecked() const
{
	bool bNoStaticMeshObjs = true;
	bool bAutoComputeLODScreenSize = false;
	bool bFirstStaticMesh = true;
	TArray<TWeakObjectPtr<UObject>> SelectedObjects;
	PropertyTable->GetSelectedTableObjects(SelectedObjects);
	for (int I = 0; I < SelectedObjects.Num(); I++)
	{
		if (!SelectedObjects[I].Get()->IsA(UStaticMesh::StaticClass()))
		{
			return ECheckBoxState::Undetermined;
		}

		UStaticMesh* StaticMesh = Cast<UStaticMesh>(SelectedObjects[I].Get());
		bNoStaticMeshObjs = false;
		//If selected staticmesh objects' bAutoComputeLODScreenSize is different, return undetermined;
		if (!bFirstStaticMesh && bAutoComputeLODScreenSize != StaticMesh->bAutoComputeLODScreenSize)
		{
			return ECheckBoxState::Undetermined;
		}
		if (bFirstStaticMesh)
		{
			bFirstStaticMesh = false;
			bAutoComputeLODScreenSize = StaticMesh->bAutoComputeLODScreenSize;
		}
	}

	if (bNoStaticMeshObjs)
	{
		return ECheckBoxState::Undetermined;
	}

	return bAutoComputeLODScreenSize ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void FPropertyEditorToolkit::HandleAutoScreensizeCheckBoxCheckedStateChanged(ECheckBoxState InNewState)
{
	if (InNewState != ECheckBoxState::Undetermined)
	{
		bool bAutoComputeLODScreenSize = InNewState == ECheckBoxState::Checked;
		GEditor->BeginTransaction(FText::FromString("Set AutoComputeLODDistances for selected staticmesh objects"));
		TArray< TWeakObjectPtr< UObject > > SelectedObjects;
		PropertyTable->GetSelectedTableObjects(SelectedObjects);
		for (int32 I = 0; I < SelectedObjects.Num(); I++)
		{
			if (SelectedObjects[I].Get()->IsA(UStaticMesh::StaticClass()))
			{
				UStaticMesh* StaticMesh = Cast<UStaticMesh>(SelectedObjects[I].Get());

				StaticMesh->Modify();
				StaticMesh->bAutoComputeLODScreenSize = bAutoComputeLODScreenSize;
			}
		}

		GEditor->EndTransaction();
	}
}


TSharedRef<SWidget> FPropertyEditorToolkit::MakeAddFilterMenu()
{
	return FilterListPtr->ExternalMakeAddFilterMenu();
}

void FPropertyEditorToolkit::OnFilterChanged()
{
	FARFilter Filter = FilterListPtr->GetCombinedBackendFilter();
	TArray<FString> AllowedClassName;
	for (const FName& ClassName : Filter.ClassNames)
	{
		AllowedClassName.Add(ClassName.ToString());
	}

	TArray<UObject*> ObjectBeingEdited = *GetObjectsCurrentlyBeingEdited();
	TArray<UObject*> AllowedObjects;
	if (AllowedClassName.Num() != 0)
	{
		for (UObject* Obj : ObjectBeingEdited)
		{
			if (AllowedClassName.Contains(Obj->GetClass()->GetName()))
			{
				AllowedObjects.Add(Obj);
			}
		}
	}
	else
	{
		AllowedObjects = ObjectBeingEdited;
	}

	//Reset the selections
	TSet< TSharedRef< IPropertyTableRow > > EmptyObject;
	PropertyTable->SetSelectedRows(EmptyObject);

	TArray< UObject* > AdjustedObjectsToEdit;
	for (auto ObjectIter = AllowedObjects.CreateConstIterator(); ObjectIter; ++ObjectIter)
	{
		//@todo Remove this and instead extend the blueprints Edit Defaults editor to use a Property Table as well [12/6/2012 Justin.Sargent]
		UObject* Object = *ObjectIter;
		if (Object->IsA(UBlueprint::StaticClass()))
		{
			UBlueprint* Blueprint = Cast<UBlueprint>(Object);

			// Make sure that the generated class is valid, in case the super has been removed, and this class can't be loaded.
			if (ensureMsgf(Blueprint->GeneratedClass != NULL, TEXT("Blueprint %s has no generated class"), *Blueprint->GetName()))
			{
				AdjustedObjectsToEdit.Add(Blueprint->GeneratedClass->GetDefaultObject());
			}
		}
		else
		{
			AdjustedObjectsToEdit.Add(Object);
		}
	}

	//Reset the property table and property tree with new objects
	PropertyTable->SetObjects(AdjustedObjectsToEdit);
	TableColumnsChanged();

	TArray< TWeakObjectPtr<UObject> > AdjustedObjectsToEditWeak;
	for (auto ObjectIter = AdjustedObjectsToEdit.CreateConstIterator(); ObjectIter; ++ObjectIter)
	{
		AdjustedObjectsToEditWeak.Add(*ObjectIter);
	}
	PropertyTree->SetObjectArray(AdjustedObjectsToEditWeak);

}


TSharedRef<SDockTab> FPropertyEditorToolkit::SpawnTab_AdditionalSettings(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == AdditionalSettingsTabId);

	TSharedRef<SDockTab> AdditionalSettingsTab = SNew(SDockTab)
		.Icon(FEditorStyle::GetBrush("PropertyEditor.Properties.TabIcon"))
		.Label(LOCTEXT("AdditionalSettings", "Additional Settings"))
		.TabColorScale(GetTabColorScale())
		.Content()
		[
			SNew(SVerticalBox)
			.Visibility(this, &FPropertyEditorToolkit::OnlySelectStaticMeshAndSkeletalMeshObjects)
			+ SVerticalBox::Slot().AutoHeight().Padding(30, 3)
			[
				SNew(SHorizontalBox)
				.Visibility(this, &FPropertyEditorToolkit::OnlySelectStaticMeshObjects)
				+ SHorizontalBox::Slot()
				.FillWidth(2)
				[
					SNew(STextBlock)
					.Text(NSLOCTEXT("PropertyEditor", "Auto LOD Distances", "Auto Compute LOD Distances"))
				]
				+ SHorizontalBox::Slot()
				.FillWidth(6)
				.HAlign(HAlign_Left)
				[
					SNew(SCheckBox)
					.IsChecked(this, &FPropertyEditorToolkit::HandleAutoScreensizeCheckBoxIsChecked)
					.OnCheckStateChanged(this, &FPropertyEditorToolkit::HandleAutoScreensizeCheckBoxCheckedStateChanged)
				]
			]

			+ SVerticalBox::Slot().AutoHeight().Padding(30, 3)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.FillWidth(2)
				[
					SNew(STextBlock)
					.Text(NSLOCTEXT("PropertyEditor", "LOD", "LOD"))
				]
				+ SHorizontalBox::Slot()
				.FillWidth(6)
				[
					SNew(SComboButton)
					.OnGetMenuContent(this, &FPropertyEditorToolkit::OnGenerateLodMenuForLodPicker)
					.VAlign(VAlign_Center)
					.ButtonContent()
					[
						SNew(STextBlock)
						.Text(this, &FPropertyEditorToolkit::GetCurrentLodName)
					]
				]
			]

			+SVerticalBox::Slot().AutoHeight().Padding(30, 3)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.FillWidth(2)
				[
					SNew(STextBlock)
					.Text(NSLOCTEXT("PropertyEditor", "ScreenSize", "Screen Size"))
				]
				+ SHorizontalBox::Slot()
				.FillWidth(6)
				[
					SNew(SNumericEntryBox<float>)
					.MinValue(0)
					.MaxValue(1)
					.Value(this, &FPropertyEditorToolkit::HandleScreenSizeEntryBoxValue)
					.OnValueCommitted(this, &FPropertyEditorToolkit::HandleScreenSizeCommit)
					.IsEnabled(true)
				]
			]


			+ SVerticalBox::Slot().AutoHeight().Padding(30, 3)
			[
				SNew(SHorizontalBox).Visibility(this, &FPropertyEditorToolkit::OnlySelectStaticMeshObjects)
				+ SHorizontalBox::Slot()
				.FillWidth(2)
				[
					SNew(STextBlock)
					.Text(NSLOCTEXT("PropertyEditor", "Triangle percent", "Percent Triangles"))
				]
				+ SHorizontalBox::Slot()
				.FillWidth(6)
				[
					SNew(SNumericEntryBox<float>)
					.MinValue(0)
					.MaxValue(1)
					.Value(this, &FPropertyEditorToolkit::HandleTrianglePercentEntryBoxValue)
					.OnValueCommitted(this, &FPropertyEditorToolkit::HandleTrianglePercentCommit)
					.IsEnabled(true)
				]
			]
		];
	return AdditionalSettingsTab;
}


FText FPropertyEditorToolkit::GetCurrentLodName() const
{
	return  FText::FromString( FString(TEXT("LOD ")) + FString::FromInt(CurLODIndex));
}

EVisibility FPropertyEditorToolkit::OnlySelectStaticMeshObjects() const
{
	bool bHasStaticMesh = false;
	TArray<TWeakObjectPtr<UObject>> SelectedObjects;
	PropertyTable->GetSelectedTableObjects(SelectedObjects);
	for (int I = 0; I < SelectedObjects.Num(); I++)
	{
		if (SelectedObjects[I].Get()->IsA(UStaticMesh::StaticClass()))
		{
			bHasStaticMesh = true;
		}
		else
		{
			return EVisibility::Collapsed;
		}
	}

	return bHasStaticMesh ? EVisibility::Visible : EVisibility::Collapsed;
}


EVisibility FPropertyEditorToolkit::OnlySelectStaticMeshAndSkeletalMeshObjects() const
{
	bool bHasStaticMeshOrSkeletalMesh = false;
	TArray<TWeakObjectPtr<UObject>> SelectedObjects;
	PropertyTable->GetSelectedTableObjects(SelectedObjects);
	for (int I = 0; I < SelectedObjects.Num(); I++)
	{
		if (SelectedObjects[I].Get()->IsA(UStaticMesh::StaticClass()) || SelectedObjects[I].Get()->IsA(USkeletalMesh::StaticClass()))
		{
			bHasStaticMeshOrSkeletalMesh = true;
		}
		else
		{
			return EVisibility::Collapsed;
		}
	}

	return bHasStaticMeshOrSkeletalMesh ? EVisibility::Visible : EVisibility::Collapsed;
}

//jarviezhang end

TSharedRef<SDockTab> FPropertyEditorToolkit::SpawnTab_PropertyTree( const FSpawnTabArgs& Args ) 
{
	check( Args.GetTabId() == TreeTabId );

	TSharedRef<SDockTab> TreeToolkitTab = SNew(SDockTab)
		.Icon( FEditorStyle::GetBrush("PropertyEditor.Properties.TabIcon") )
		.Label( LOCTEXT("GenericDetailsTitle", "Details") )
		.TabColorScale( GetTabColorScale() )
		.Content()
		[
			SNew(SBorder)
			.Padding(4)
			.BorderImage( FEditorStyle::GetBrush( "ToolPanel.GroupBorder" ) )
			.Content()
			[
				PropertyTree.ToSharedRef()
			]
		];	
	
	return TreeToolkitTab;
}


TSharedRef<SDockTab> FPropertyEditorToolkit::SpawnTab_PropertyTable( const FSpawnTabArgs& Args ) 
{
	check( Args.GetTabId() == GridTabId );

	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>( "PropertyEditor" );
	TSharedRef<SDockTab> GridToolkitTab = SNew(SDockTab)
		.Icon( FEditorStyle::GetBrush("PropertyEditor.Grid.TabIcon") )
		.Label( LOCTEXT("GenericGridTitle", "Grid") )
		.TabColorScale( GetTabColorScale() )
		.Content()
		[
			SNew( SOverlay )
			+SOverlay::Slot()
			[
				PropertyEditorModule.CreatePropertyTableWidget( PropertyTable.ToSharedRef() )
			]
			+SOverlay::Slot()
			.HAlign( HAlign_Right )
			.VAlign( VAlign_Top )
			.Padding( FMargin( 0, 3, 0, 0 ) )
			[
				SNew( SHorizontalBox )
				+SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign( HAlign_Center )
				.VAlign( VAlign_Center )
				[
					SNew( SImage )
					.Image( FEditorStyle::GetBrush( "PropertyEditor.AddColumnOverlay" ) )
					.Visibility( this, &FPropertyEditorToolkit::GetAddColumnInstructionsOverlayVisibility )
				]

				+SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign( HAlign_Center )
				.VAlign( VAlign_Center )
				[
					SNew( SImage )
					.Image( FEditorStyle::GetBrush( "PropertyEditor.RemoveColumn" ) )
					.Visibility( this, &FPropertyEditorToolkit::GetAddColumnInstructionsOverlayVisibility )
				]

				+SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign( HAlign_Center )
				.VAlign( VAlign_Center )
				.Padding( FMargin( 0, 0, 3, 0 ) )
				[
					SNew( STextBlock )
					.Font( FEditorStyle::GetFontStyle( "PropertyEditor.AddColumnMessage.Font" ) )
					.Text( LOCTEXT("GenericPropertiesTitle", "Pin Properties to Add Columns") )
					.Visibility( this, &FPropertyEditorToolkit::GetAddColumnInstructionsOverlayVisibility )
					.ColorAndOpacity( FEditorStyle::GetColor( "PropertyEditor.AddColumnMessage.ColorAndOpacity" ) )
				]
			]
		];	

	return GridToolkitTab;
}


void FPropertyEditorToolkit::CreatePropertyTree()
{
	PropertyTree = SNew( SPropertyTreeViewImpl )
		.AllowFavorites( false )
		.ShowTopLevelNodes(false)
		.OnPropertyMiddleClicked( this, &FPropertyEditorToolkit::ToggleColumnForProperty )
		.ConstructExternalColumnHeaders( this, &FPropertyEditorToolkit::ConstructTreeColumns )
		.ConstructExternalColumnCell( this, &FPropertyEditorToolkit::ConstructTreeCell )
		.NameColumnWidth( 0.5f );
}


void FPropertyEditorToolkit::CreatePropertyTable()
{
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>( "PropertyEditor" );
	PropertyTable = PropertyEditorModule.CreatePropertyTable();

	PropertyTable->OnSelectionChanged()->AddSP( this, &FPropertyEditorToolkit::GridSelectionChanged );
	PropertyTable->OnColumnsChanged()->AddSP( this, &FPropertyEditorToolkit::TableColumnsChanged );
	PropertyTable->OnRootPathChanged()->AddSP( this, &FPropertyEditorToolkit::GridRootPathChanged );
}


void FPropertyEditorToolkit::ConstructTreeColumns( const TSharedRef< class SHeaderRow >& HeaderRow )
{
	SHeaderRow::FColumn::FArguments ColumnArgs;
	ColumnArgs
	.ColumnId( TreePinAsColumnHeaderId )
	.FixedWidth(24)
	[
		SNew(SBorder)
		.Padding( 0 )
		.BorderImage( FEditorStyle::GetBrush( "NoBorder" ) )
		.HAlign( HAlign_Center )
		.VAlign( VAlign_Center )
		.ToolTipText( LOCTEXT("AddColumnLabel", "Push Pins to Add Columns") )
		[
			SNew( SImage )
			.Image( FEditorStyle::GetBrush(TEXT("PropertyEditor.RemoveColumn")) )
		]
	];

	HeaderRow->InsertColumn( ColumnArgs, 0 );
}


TSharedRef< SWidget > FPropertyEditorToolkit::ConstructTreeCell( const FName& ColumnName, const TSharedRef< IPropertyTreeRow >& Row )
{
	if ( ColumnName == TreePinAsColumnHeaderId )
	{
		const TWeakPtr<IPropertyTreeRow> RowPtr = Row;
		PinRows.Add( Row );

		return SNew( SBorder )
			.Padding( 0 )
			.BorderImage( &FEditorStyle::GetWidgetStyle<FHeaderRowStyle>("PropertyTable.HeaderRow").ColumnStyle.NormalBrush )
			[
				SNew(SButton)
				.IsFocusable(false)
				.ToolTipText(NSLOCTEXT("PropertyEditor", "ToggleColumnButtonToolTip", "Toggle Column"))
				.ButtonStyle( FEditorStyle::Get(), "NoBorder" )
				.ContentPadding(0) 
				.OnClicked( this, &FPropertyEditorToolkit::OnToggleColumnClicked, RowPtr )
				.HAlign( HAlign_Center )
				.VAlign( VAlign_Center )
				[
					SNew(SImage)
					.Image( this, &FPropertyEditorToolkit::GetToggleColumnButtonImageBrush, RowPtr )
					.ColorAndOpacity( this, &FPropertyEditorToolkit::GetPinColorAndOpacity, RowPtr )
				]
			];
	}

	return SNullWidget::NullWidget;
}


EVisibility FPropertyEditorToolkit::GetAddColumnInstructionsOverlayVisibility() const
{
	return TableHasCustomColumns() ? EVisibility::Collapsed : EVisibility::HitTestInvisible;
}


void FPropertyEditorToolkit::ToggleColumnForProperty( const TSharedPtr< FPropertyPath >& PropertyPath )
{
	if ( !PropertyPath.IsValid() )
	{
		return;
	}

	TSharedRef< FPropertyPath > NewPath = PropertyPath->TrimRoot( PropertyTable->GetRootPath()->GetNumProperties() );
	const TSet< TSharedRef< IPropertyTableRow > > SelectedRows = PropertyTable->GetSelectedRows();
	
	for( auto RowIter = SelectedRows.CreateConstIterator(); RowIter; ++RowIter )
	{
		NewPath = NewPath->TrimRoot( (*RowIter)->GetPartialPath()->GetNumProperties() );
		break;
	}

	if ( NewPath->GetNumProperties() == 0 )
	{
		return;
	}

	TSharedPtr< IPropertyTableColumn > ExistingColumn;
	for( auto ColumnIter = PropertyTable->GetColumns().CreateConstIterator(); ColumnIter; ++ColumnIter )
	{
		TSharedRef< IPropertyTableColumn > Column = *ColumnIter;
		const TSharedPtr< FPropertyPath > Path = Column->GetDataSource()->AsPropertyPath();

		if ( Path.IsValid() && FPropertyPath::AreEqual( Path.ToSharedRef(), NewPath ) )
		{
			ExistingColumn = Column;
		}
	}

	if ( ExistingColumn.IsValid() )
	{
		PropertyTable->RemoveColumn( ExistingColumn.ToSharedRef() );
		const TSharedRef< FPropertyPath > ColumnPath = ExistingColumn->GetDataSource()->AsPropertyPath().ToSharedRef();
		for (int Index = PropertyPathsAddedAsColumns.Num() - 1; Index >= 0 ; Index--)
		{
			if ( FPropertyPath::AreEqual( ColumnPath, PropertyPathsAddedAsColumns[ Index ] ) )
			{
				PropertyPathsAddedAsColumns.RemoveAt( Index );
			}
		}
	}
	else
	{
		PropertyTable->AddColumn( NewPath );
		PropertyPathsAddedAsColumns.Add( NewPath );
	}
}


bool FPropertyEditorToolkit::TableHasCustomColumns() const
{
	return PropertyPathsAddedAsColumns.Num() > 0;
}

bool FPropertyEditorToolkit::CloseWindow()
{
	GEditor->GetTimerManager()->ClearTimer( TimerHandle_TickPinColor );
	return FAssetEditorToolkit::CloseWindow();
}


bool FPropertyEditorToolkit::IsExposedAsColumn( const TWeakPtr< IPropertyTreeRow >& Row ) const
{
	bool Result = false;

	if (Row.IsValid())
	{
		const TSharedPtr< FPropertyPath > RowPathPtr = Row.Pin()->GetPropertyPath();
		if ( RowPathPtr.IsValid() )
		{
			TSharedRef< FPropertyPath > TrimmedPath = RowPathPtr->TrimRoot( PropertyTable->GetRootPath()->GetNumProperties() );
			const TSet< TSharedRef< IPropertyTableRow > > SelectedRows = PropertyTable->GetSelectedRows();

			for( auto RowIter = SelectedRows.CreateConstIterator(); RowIter; ++RowIter )
			{
				TrimmedPath = TrimmedPath->TrimRoot( (*RowIter)->GetPartialPath()->GetNumProperties() );
				break;
			}

			for (int Index = 0; Index < PropertyPathsAddedAsColumns.Num(); Index++)
			{
				if ( FPropertyPath::AreEqual( TrimmedPath, PropertyPathsAddedAsColumns[ Index ] ) )
				{
					Result = true;
					break;
				}
			}
		}
	}

	return Result;
}

void FPropertyEditorToolkit::TableColumnsChanged()
{
	PropertyPathsAddedAsColumns.Empty();

	for( auto ColumnIter = PropertyTable->GetColumns().CreateConstIterator(); ColumnIter; ++ColumnIter )
	{
		TSharedRef< IPropertyTableColumn > Column = *ColumnIter;
		TSharedPtr< FPropertyPath > ColumnPath = Column->GetDataSource()->AsPropertyPath();

		if ( ColumnPath.IsValid() && ColumnPath->GetNumProperties() > 0 )
		{
			PropertyPathsAddedAsColumns.Add( ColumnPath.ToSharedRef() );
		}
	}
}


void FPropertyEditorToolkit::GridSelectionChanged()
{
	TArray< TWeakObjectPtr< UObject > > SelectedObjects;
	PropertyTable->GetSelectedTableObjects( SelectedObjects );
	PropertyTree->SetObjectArray( SelectedObjects );

	const TSet< TSharedRef< IPropertyTableRow > > SelectedRows = PropertyTable->GetSelectedRows();

	if ( SelectedRows.Num() == 1 )
	{
		for( auto RowIter = SelectedRows.CreateConstIterator(); RowIter; ++RowIter )
		{
			PropertyTree->SetRootPath( PropertyTable->GetRootPath()->ExtendPath( (*RowIter)->GetPartialPath() ) );
			break;
		}
	}
	else if ( !FPropertyPath::AreEqual( PropertyTree->GetRootPath(), PropertyTable->GetRootPath() ) )
	{
		PropertyTree->SetRootPath( PropertyTable->GetRootPath() );
	}
}


void FPropertyEditorToolkit::GridRootPathChanged()
{
	GridSelectionChanged();
	PropertyTree->SetRootPath( PropertyTable->GetRootPath() );
}

FName FPropertyEditorToolkit::GetToolkitFName() const
{
	return ToolkitFName;
}

FText FPropertyEditorToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "Property Editor");
}

FText FPropertyEditorToolkit::GetToolkitName() const
{
	const TArray<UObject*>& EditingObjs = GetEditingObjects();

	int32 NumEditingObjects = EditingObjs.Num();

	check( NumEditingObjects > 0 );

	if( NumEditingObjects == 1 )
	{
		const UObject* EditingObject = EditingObjs[ 0 ];

		const bool bDirtyState = EditingObject->GetOutermost()->IsDirty();

		FFormatNamedArguments Args;
		Args.Add( TEXT("ObjectName"), FText::FromString( EditingObject->GetName() ) );
		Args.Add( TEXT("DirtyState"), bDirtyState ? FText::FromString( TEXT( "*" ) ) : FText::GetEmpty() );
		Args.Add( TEXT("ToolkitName"), GetBaseToolkitName() );
		return FText::Format( LOCTEXT("ToolkitName_SingleObject", "{ObjectName}{DirtyState} - {ToolkitName}"), Args );
	}
	else
	{
		bool bDirtyState = false;
		UClass* SharedBaseClass = NULL;
		for( int32 x = 0; x < NumEditingObjects; ++x )
		{
			UObject* Obj = EditingObjs[ x ];
			check( Obj );

			UClass* ObjClass = Cast<UClass>(Obj);
			if (ObjClass == NULL)
			{
				ObjClass = Obj->GetClass();
			}
			check( ObjClass );

			// Initialize with the class of the first object we encounter.
			if( SharedBaseClass == NULL )
			{
				SharedBaseClass = ObjClass;
			}

			// If we've encountered an object that's not a subclass of the current best baseclass,
			// climb up a step in the class hierarchy.
			while( !ObjClass->IsChildOf( SharedBaseClass ) )
			{
				SharedBaseClass = SharedBaseClass->GetSuperClass();
			}

			// If any of the objects are dirty, flag the label
			bDirtyState |= Obj->GetOutermost()->IsDirty();
		}

		FFormatNamedArguments Args;
		Args.Add( TEXT("NumberOfObjects"), EditingObjs.Num() );
		Args.Add( TEXT("ClassName"), FText::FromString( SharedBaseClass->GetName() ) );
		Args.Add( TEXT("DirtyState"), bDirtyState ? FText::FromString( TEXT( "*" ) ) : FText::GetEmpty() );
		return FText::Format( LOCTEXT("ToolkitName_MultiObject", "{NumberOfObjects} {ClassName}{DirtyState} Objects - Property Matrix Editor"), Args );
	}
}


FText FPropertyEditorToolkit::GetToolkitToolTipText() const
{
	const TArray<UObject*>& EditingObjs = GetEditingObjects();

	int32 NumEditingObjects = EditingObjs.Num();

	check( NumEditingObjects > 0 );

	if( NumEditingObjects == 1 )
	{
		const UObject* EditingObject = EditingObjs[ 0 ];
		return FAssetEditorToolkit::GetToolTipTextForObject(EditingObject);
	}
	else
	{
		UClass* SharedBaseClass = NULL;
		for( int32 x = 0; x < NumEditingObjects; ++x )
		{
			UObject* Obj = EditingObjs[ x ];
			check( Obj );

			UClass* ObjClass = Cast<UClass>(Obj);
			if (ObjClass == NULL)
			{
				ObjClass = Obj->GetClass();
			}
			check( ObjClass );

			// Initialize with the class of the first object we encounter.
			if( SharedBaseClass == NULL )
			{
				SharedBaseClass = ObjClass;
			}

			// If we've encountered an object that's not a subclass of the current best baseclass,
			// climb up a step in the class hierarchy.
			while( !ObjClass->IsChildOf( SharedBaseClass ) )
			{
				SharedBaseClass = SharedBaseClass->GetSuperClass();
			}
		}

		FFormatNamedArguments Args;
		Args.Add( TEXT("NumberOfObjects"), NumEditingObjects );
		Args.Add( TEXT("ClassName"), FText::FromString( SharedBaseClass->GetName() ) );
		return FText::Format( LOCTEXT("ToolkitName_MultiObjectToolTip", "{NumberOfObjects} {ClassName} Objects - Property Matrix Editor"), Args );
	}
}


FReply FPropertyEditorToolkit::OnToggleColumnClicked( const TWeakPtr< IPropertyTreeRow > Row )
{
	if (Row.IsValid())
	{
		ToggleColumnForProperty( Row.Pin()->GetPropertyPath() );
	}

	return FReply::Handled();
}


const FSlateBrush* FPropertyEditorToolkit::GetToggleColumnButtonImageBrush( const TWeakPtr< IPropertyTreeRow > Row ) const
{
	if ( IsExposedAsColumn( Row ) )
	{
		return FEditorStyle::GetBrush("PropertyEditor.RemoveColumn");
	}

	return FEditorStyle::GetBrush("PropertyEditor.AddColumn");
}

void FPropertyEditorToolkit::TickPinColorAndOpacity()
{
	bool IsRowBeingHoveredOver = false;
	for (int Index = PinRows.Num() - 1; Index >= 0 ; Index--)
	{
		TSharedPtr< IPropertyTreeRow > Row = PinRows[ Index ].Pin();
		if ( Row.IsValid() )
		{
			IsRowBeingHoveredOver |= Row->IsCursorHovering();

			if ( IsRowBeingHoveredOver )
			{
				break;
			}
		}
		else
		{
			PinRows.RemoveAt( Index );
		}
	}

	if ( IsRowBeingHoveredOver )
	{
		PinSequence.JumpToStart();
	}

	float Opacity = 0.0f;
	if ( !TableHasCustomColumns() )
	{
		Opacity = PinSequence.GetLerp();
	}

	if ( !PinSequence.IsPlaying() )
	{
		if ( PinSequence.IsAtStart() )
		{
			PinSequence.Play( PropertyTree.ToSharedRef() );
		}
		else
		{
			PinSequence.PlayReverse( PropertyTree.ToSharedRef() );
		}
	}

	PinColor = FSlateColor( FColor( 255, 255, 255, FMath::Lerp( 0, 200, Opacity ) ).ReinterpretAsLinear() );
}

FSlateColor FPropertyEditorToolkit::GetPinColorAndOpacity( const TWeakPtr< IPropertyTreeRow > Row ) const
{
	if ( Row.IsValid() && ( Row.Pin()->IsCursorHovering() || IsExposedAsColumn( Row ) ) )
	{
		return FSlateColor( FLinearColor::White );
	}

	return PinColor;
}


FString FPropertyEditorToolkit::GetWorldCentricTabPrefix() const
{
	check(0);
	return TEXT("");
}


FLinearColor FPropertyEditorToolkit::GetWorldCentricTabColorScale() const
{
	check(0);
	return FLinearColor();
}

#undef LOCTEXT_NAMESPACE
