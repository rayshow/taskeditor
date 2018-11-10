// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "Layout/Visibility.h"
#include "Animation/CurveSequence.h"
#include "Styling/SlateColor.h"
#include "Input/Reply.h"
#include "Widgets/SWidget.h"
#include "PropertyPath.h"
#include "Toolkits/IToolkitHost.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "IPropertyTable.h"
#include "SPropertyTreeViewImpl.h"

//jarviezhang begin
#include "Editor/ContentBrowser/Public/IContentBrowserSingleton.h"
//jarviezhang end

//jarviezhang begin
class SFilterList;
//jarviezhang end
class IPropertyTreeRow;

class FPropertyEditorToolkit : public FAssetEditorToolkit
{
public:

	FPropertyEditorToolkit();

	virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;

	virtual void UnregisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;

	virtual FName GetToolkitFName() const override;

	virtual FText GetBaseToolkitName() const override;

	virtual FText GetToolkitName() const override;

	virtual FText GetToolkitToolTipText() const override;

	virtual FString GetWorldCentricTabPrefix() const override;

	virtual FLinearColor GetWorldCentricTabColorScale() const override;

	bool IsExposedAsColumn( const TWeakPtr< IPropertyTreeRow >& Row ) const;

	void ToggleColumnForProperty( const TSharedPtr< class FPropertyPath >& PropertyPath );

	bool TableHasCustomColumns() const;

	virtual bool CloseWindow() override;

	virtual bool IsPrimaryEditor() const override{ return false; };

public:

	static TSharedRef<FPropertyEditorToolkit> CreateEditor( const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UObject* ObjectToEdit );

	static TSharedRef<FPropertyEditorToolkit> CreateEditor( const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, const TArray<UObject*>& ObjectsToEdit );


private:

	//jarviezhang begin
	//Add a filter 
	/** The filter list */
	TSharedPtr<SFilterList> FilterListPtr;

	/** The list of FrontendFilters currently applied to the asset view */
	TSharedPtr<FAssetFilterCollectionType> FrontendFilters;

	void OnFilterChanged();

	//Add filter combo button to toolbar
	void AddFilterButton(FToolBarBuilder& Builder);

	//Generate the filter list when click the "filter" button
	TSharedRef<SWidget> MakeAddFilterMenu();

	//jarviezhang end

	//jarviezhang begin :Add AdditionalSettings tab
	int32 CurLODIndex;

	void HandleLODEntryBoxChanged(int32 LODIndex);
	TOptional<int32> HandleLODEntryBoxValue() const;
	TSharedRef<SWidget> OnGenerateLodMenuForLodPicker();

	TOptional<float> HandleScreenSizeEntryBoxValue() const;
	void HandleScreenSizeCommit(const float Value, ETextCommit::Type CommitType);

	TOptional<float> HandleTrianglePercentEntryBoxValue() const;
	void HandleTrianglePercentCommit(const float Value, ETextCommit::Type CommitType);

	ECheckBoxState HandleAutoScreensizeCheckBoxIsChecked() const;
	void HandleAutoScreensizeCheckBoxCheckedStateChanged(ECheckBoxState InNewState);

	FText GetCurrentLodName() const;
	EVisibility OnlySelectStaticMeshObjects() const;
	EVisibility OnlySelectStaticMeshAndSkeletalMeshObjects() const;

	void SetStaticMeshLODScreenSize(UObject* Obj, float ScreenSize);
	void SetSkeletalMeshLODScreenSize(UObject* Obj, float ScreenSize);
	//jarviezhang end

	static TSharedPtr<FPropertyEditorToolkit> FindExistingEditor( UObject* Object );

	void Initialize( const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, const TArray<UObject*>& ObjectsToEdit );

	void CreatePropertyTree();

	void CreatePropertyTable();

	void CreateGridView();

	//jarviezhang begin
	TSharedRef<SDockTab> SpawnTab_AdditionalSettings(const FSpawnTabArgs& Args);
	//jarviezhang end

	TSharedRef<SDockTab> SpawnTab_PropertyTree( const FSpawnTabArgs& Args );

	TSharedRef<SDockTab> SpawnTab_PropertyTable( const FSpawnTabArgs& Args ) ;

	void GridSelectionChanged();

	void GridRootPathChanged();

	void ConstructTreeColumns( const TSharedRef< class SHeaderRow >& HeaderRow );

	TSharedRef< SWidget > ConstructTreeCell( const FName& ColumnName, const TSharedRef< class IPropertyTreeRow >& Row );

	FReply OnToggleColumnClicked( const TWeakPtr< class IPropertyTreeRow > Row );

	const FSlateBrush* GetToggleColumnButtonImageBrush( const TWeakPtr< class IPropertyTreeRow > Row ) const;

	EVisibility GetToggleColumnButtonVisibility( const TSharedRef< class IPropertyTreeRow > Row ) const;

	void TickPinColorAndOpacity();

	FSlateColor GetPinColorAndOpacity( const TWeakPtr< IPropertyTreeRow > Row ) const;

	void TableColumnsChanged();

	EVisibility GetAddColumnInstructionsOverlayVisibility() const;


private:

	TSharedPtr< class SPropertyTreeViewImpl > PropertyTree;
	TSharedPtr< class IPropertyTable > PropertyTable;

	TSharedPtr< FPropertyPath > PathToRoot;

	TArray< TSharedRef< FPropertyPath > > PropertyPathsAddedAsColumns;

	/** Animation sequence to pulse the pin image */
	FCurveSequence PinSequence;
	FTimerHandle TimerHandle_TickPinColor;
	FSlateColor PinColor;
	TArray< TWeakPtr<IPropertyTreeRow> > PinRows;

	static const FName ToolkitFName;

	static const FName ApplicationId;
	static const FName TreeTabId;
	static const FName GridTabId;

	//jarviezhang begin
	static const FName AdditionalSettingsTabId;
	//jarviezhang end
	static const FName TreePinAsColumnHeaderId;
};
