// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "SlateFwd.h"
#include "Layout/Visibility.h"
#include "Input/Reply.h"
#include "PropertyPath.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "IPropertyUtilities.h"
#include "Widgets/Views/STableViewBase.h"
#include "Widgets/Views/STableRow.h"
#include "Widgets/Views/STreeView.h"
#include "PropertyEditorDelegates.h"

class AActor;
class FNotifyHook;
class FObjectPropertyNode;
class FPropertyNode;

typedef STreeView< TSharedPtr<FPropertyNode> > SPropertyTree;


class SPropertyTemplateImpl : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPropertyTemplateImpl)
		: _IsLockable(true)
		, _HiddenPropertyVis(false)
		, _AllowSearch(true)
		, _ShowTopLevelNodes(true)
		, _NotifyHook(NULL)
		, _NameColumnWidth(0.20f)
		, _OnPropertySelectionChanged()
		, _OnPropertyMiddleClicked()
		, _ConstructExternalColumnHeaders()
		, _ConstructExternalColumnCell()
	{}

		SLATE_ARGUMENT(bool, IsLockable)
		SLATE_ARGUMENT(bool, HiddenPropertyVis)
		SLATE_ARGUMENT(bool, AllowSearch)
		SLATE_ARGUMENT(bool, ShowTopLevelNodes)
		SLATE_ARGUMENT(FNotifyHook*, NotifyHook)
		SLATE_ARGUMENT(float, NameColumnWidth)
		SLATE_EVENT(FOnPropertySelectionChanged, OnPropertySelectionChanged)
		SLATE_EVENT(FOnPropertyClicked, OnPropertyMiddleClicked)
		SLATE_EVENT(FConstructExternalColumnHeaders, ConstructExternalColumnHeaders)
		SLATE_EVENT(FConstructExternalColumnCell, ConstructExternalColumnCell)
	SLATE_END_ARGS()

	SPropertyTemplateImpl();
	~SPropertyTemplateImpl();

	/**
	* Constructs the property view widgets
	*/
	void Construct(const FArguments& InArgs);

	/** Reconstructs the entire property tree widgets */
	void ConstructPropertyTree();

	void OnFilterTextChanged(const FText& InFilterText);

	virtual FNotifyHook* GetNotifyHook() const { return NotifyHook; }

	virtual void CreateColorPickerWindow(const TSharedRef< class FPropertyEditor >& PropertyEditor, bool bUseAlpha);

	virtual void EnqueueDeferredAction(FSimpleDelegate& DeferredAction);

	virtual void RequestRefresh();

	virtual void SetFilterText(const FText& InFilterText);

	virtual void SetObjectArray(const TArray< TWeakObjectPtr< UObject > >& InObjects);

	virtual void SetRootPath(const TSharedPtr< FPropertyPath >& Path);

	virtual TSharedRef< FPropertyPath > GetRootPath() const;

private:

	void FilterView(const FString& InFilterText);

	void OnGetChildrenForPropertyNode(TSharedPtr<FPropertyNode> InPropertyNode, TArray< TSharedPtr<FPropertyNode> >& OutChildren);

	TSharedRef<ITableRow> OnGenerateRowForPropertyTree(TSharedPtr<FPropertyNode> InPropertyNode, const TSharedRef<STableViewBase>& OwnerTable);

	void OnSelectionChanged(TSharedPtr<FPropertyNode> InPropertyNode, ESelectInfo::Type SelectInfo);

	void RequestItemExpanded(TSharedPtr<FPropertyNode> PropertyNode, bool bExpand, bool bRecursiveExpansion = false);

	void UpdateTopLevelPropertyNodes(TSharedPtr<FPropertyNode> FirstVisibleNode);

	TSharedRef<ITableRow> CreatePropertyEditor(TSharedPtr<FPropertyNode> InPropertyNode, const TSharedPtr<STableViewBase>& OwnerTable);

	void SetColor(FLinearColor NewColor);

	void PreSetObject();

	void PostSetObject();

	virtual void SaveExpandedItems();

	virtual void SaveColumnWidths();

private:

	/** Stored set of expanded nodes before a filter was set */
	TSet< TSharedPtr<FPropertyNode> > PreFilterExpansionSet;
	/** Map of nodes that are requesting an automatic expansion/collapse due to being filtered */
	TMap< TSharedPtr<FPropertyNode>, bool > FilteredNodesRequestingExpansionState;
	/** Top level property nodes which are visible in the TreeView. These are always category nodes belonging to the RootPropertyNode */
	TArray< TSharedPtr<FPropertyNode> > TopLevelPropertyNodes;
 
	/** Actions that should be executed next tick */
	TArray<FSimpleDelegate> DeferredActions;
	/** The root property node of the property tree for a specific set of UObjects */
	TSharedPtr<FObjectPropertyNode> RootPropertyNode;
	/** Our property treeview */
	TSharedPtr<SPropertyTree> PropertyTree;
	/** The filter text box */
	TSharedPtr<SSearchBox> FilterTextBox;
	/** The header row for the primary trees columns */
	TSharedPtr< SHeaderRow > ColumnHeaderRow;
	/** Settings for this property view */
	TSharedPtr<class IPropertyUtilities> PropertySettings;
	/** The current filter text */
	FString CurrentFilterText;
	/** The title of the property view (for a window title) */
	FString Title;
	/** The property node that the color picker is currently editing. */
	FPropertyNode* ColorPropertyNode;
	/** Notify hook to call when properties are changed */
	FNotifyHook* NotifyHook;
	/** True if there is an active filter (text in the filter box) */
	bool bHasActiveFilter;
	/** True if we allow searching */
	bool bAllowSearch;
	/** True if the property view shows all properties regardless of their flags */
	bool bForceHiddenPropertyVisibility;
	/** Whether or not this tree view manages and creates property nodes or whether the nodes are externally managed */
	bool bNodeTreeExternallyManaged;
	/** Whether or not this tree should ever display top level property nodes as categories */
	bool bShowTopLevelPropertyNodes;
	/** Callback to send when the property view changes */
	FOnObjectArrayChanged OnObjectArrayChanged;
	/** Callback when property selection changes. */
	FOnPropertySelectionChanged OnPropertySelectionChanged;
	/**	Callback when a property is clicked with the middle mouse button by the user  */
	FOnPropertyClicked OnPropertyMiddleClicked;
	/** Callback to see if a property is visible */
	FIsPropertyVisible IsPropertyVisible;
	/** The initial width of the name column */
	float InitialNameColumnWidth;
	/** The path to the property node that should be treated as the root */
	TSharedRef< class FPropertyPath > RootPath;


};