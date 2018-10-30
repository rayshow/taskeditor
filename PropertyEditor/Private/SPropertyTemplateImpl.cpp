#include"SPropertyTemplateImpl.h"
#include "PropertyNode.h"
#include "ObjectPropertyNode.h"
#include "EngineGlobals.h"
#include "GameFramework/Actor.h"
#include "Engine/Engine.h"
#include "EditorStyleSet.h"
#include "Presentation/PropertyEditor/PropertyEditor.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"

#include "CategoryPropertyNode.h"
#include "UserInterface/PropertyTree/PropertyTreeConstants.h"

#include "UserInterface/PropertyEditor/SPropertyEditorTableRow.h"
#include "UserInterface/PropertyTree/SPropertyTreeCategoryRow.h"
#include "ScopedTransaction.h"
#include "PropertyEditorHelpers.h"
#include "Misc/ConfigCacheIni.h"
#include "Widgets/Colors/SColorPicker.h"
#include "Widgets/Input/SSearchBox.h"

class FPropertyUtilitiesTemplate : public IPropertyUtilities
{
public:

	FPropertyUtilitiesTemplate(SPropertyTemplateImpl& InView)
		: View(InView)
	{
	}

	virtual class FNotifyHook* GetNotifyHook() const override
	{
		return View.GetNotifyHook();
	}

	virtual bool AreFavoritesEnabled() const override { return false; }

	virtual void ToggleFavorite(const TSharedRef< class FPropertyEditor >& PropertyEditor) const override{}

	virtual void CreateColorPickerWindow(const TSharedRef< class FPropertyEditor >& PropertyEditor, bool bUseAlpha) const override
	{
		View.CreateColorPickerWindow(PropertyEditor, bUseAlpha);
	}

	virtual void EnqueueDeferredAction(FSimpleDelegate DeferredAction) override
	{
		View.EnqueueDeferredAction(DeferredAction);
	}

	virtual void ForceRefresh() override
	{
		RequestRefresh();
	}

	virtual void RequestRefresh() override
	{
		View.RequestRefresh();
	}

	virtual bool IsPropertyEditingEnabled() const override
	{
		return true;
	}

	virtual TSharedPtr<class FAssetThumbnailPool> GetThumbnailPool() const override
	{
		return NULL;
	}

	virtual void NotifyFinishedChangingProperties(const FPropertyChangedEvent& PropertyChangedEvent) override {}

	virtual bool DontUpdateValueWhileEditing() const override
	{
		return false;
	}

	const TArray<TWeakObjectPtr<UObject>>& GetSelectedObjects() const override
	{
		static TArray<TWeakObjectPtr<UObject>> NotSupported;
		return NotSupported;
	}

	virtual bool HasClassDefaultObject() const override
	{
		return false;
	}
private:

	SPropertyTemplateImpl & View;
};


SPropertyTemplateImpl::SPropertyTemplateImpl()
	:RootPath(FPropertyPath::CreateEmpty())
{
	FPropertyNode::DisplayCheckBox = true;
}

SPropertyTemplateImpl::~SPropertyTemplateImpl()
{
	FPropertyNode::DisplayCheckBox = false;
}

void SPropertyTemplateImpl::Construct(const FArguments& InArgs)
{ 
	bHasActiveFilter = false; 
	bAllowSearch = InArgs._AllowSearch; 
	bShowTopLevelPropertyNodes = InArgs._ShowTopLevelNodes;
	NotifyHook = InArgs._NotifyHook;
	bForceHiddenPropertyVisibility = InArgs._HiddenPropertyVis;
	InitialNameColumnWidth = InArgs._NameColumnWidth;
	bNodeTreeExternallyManaged = false;
	OnPropertySelectionChanged = InArgs._OnPropertySelectionChanged;
	OnPropertyMiddleClicked = InArgs._OnPropertyMiddleClicked;


	RootPropertyNode = MakeShareable(new FObjectPropertyNode);

	PropertySettings = MakeShareable(new FPropertyUtilitiesTemplate(*this));

	ConstructPropertyTree();
}

void SPropertyTemplateImpl::ConstructPropertyTree()
{
	const FString OldFilterText = CurrentFilterText;
	CurrentFilterText.Empty();
	 
	PropertyTree.Reset();
	FilterTextBox.Reset();

	// Don't pad area around the search bar if we aren't showing anything in that area
	float PaddingBeforeFilter = bAllowSearch ? 5.0f : 0.0f;
	float PaddingAfterFilter = bAllowSearch  ? 10.0f : 0.0f;

	ESelectionMode::Type SelectionMode = ESelectionMode::None;
	if (OnPropertySelectionChanged.IsBound())
	{
		SelectionMode = ESelectionMode::Single;
	}

	this->ChildSlot
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.VAlign(VAlign_Fill)
			.Padding(PaddingBeforeFilter)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Fill)
				.FillWidth(1)
				.Padding(0, 0, 3, 0)
				[
					SAssignNew(FilterTextBox, SSearchBox)
					.Visibility(bAllowSearch ? EVisibility::Visible : EVisibility::Collapsed)
					.OnTextChanged(this, &SPropertyTemplateImpl::OnFilterTextChanged)
				]
			]
			+ SVerticalBox::Slot()
			.VAlign(VAlign_Fill)
			.FillHeight(1)
			.Padding(0.0f, PaddingAfterFilter, 0.0f, 0.0f)
			[
				SAssignNew(PropertyTree, SPropertyTree)
				.ItemHeight(PropertyTreeConstants::ItemHeight)
				.TreeItemsSource(&TopLevelPropertyNodes)
				.OnGetChildren(this, &SPropertyTemplateImpl::OnGetChildrenForPropertyNode)
				.OnGenerateRow(this, &SPropertyTemplateImpl::OnGenerateRowForPropertyTree)
				.OnSelectionChanged(this, &SPropertyTemplateImpl::OnSelectionChanged)
				.SelectionMode(SelectionMode)
				.HeaderRow
				(
					SAssignNew(ColumnHeaderRow, SHeaderRow)
					+ SHeaderRow::Column(PropertyTreeConstants::ColumnId_Name)
					.FillWidth(InitialNameColumnWidth)
					[
						SNew(SBorder)
						.Padding(3)
						.BorderImage(FEditorStyle::GetBrush("NoBorder"))
						[
							SNew(STextBlock)
							.Text(NSLOCTEXT("PropertyEditor", "NameColumn", "Name"))
						]
					]
					+ SHeaderRow::Column(PropertyTreeConstants::ColumnId_Property)
					.FillWidth(1.0f)
					[
						SNew(SBorder)
						.Padding(3)
						.BorderImage(FEditorStyle::GetBrush("NoBorder"))
						[
							SNew(STextBlock)
							.Text(NSLOCTEXT("PropertyEditor", "PropertyColumn", "Value"))
						]
					]
				)
			]
		];

	// If we had an old filter, restore it.
	if (!OldFilterText.IsEmpty())
	{
		SetFilterText(FText::FromString(OldFilterText));
	}
}


void SPropertyTemplateImpl::OnFilterTextChanged(const FText& InFilterText)
{
	const bool bFilterCleared = InFilterText.ToString().Len() == 0 && CurrentFilterText.Len() > 0;
	const bool bFilterJustActivated = CurrentFilterText.Len() == 0 && InFilterText.ToString().Len() > 0;

	CurrentFilterText = InFilterText.ToString();

	if (bFilterJustActivated)
	{
		// Store off the expanded items when starting a new filter
		// We will restore them after the filter is cleared
		PreFilterExpansionSet.Empty();
		PropertyTree->GetExpandedItems(PreFilterExpansionSet);
	}

	FilterView(CurrentFilterText);

	if (bFilterCleared)
	{
		// Clear the current expanded state
		PropertyTree->ClearExpandedItems();

		// Restore previously expanded items
		for (TSet< TSharedPtr<FPropertyNode> >::TConstIterator It(PreFilterExpansionSet); It; ++It)
		{
			PropertyTree->SetItemExpansion(*It, true);
		}
	}
}


void SPropertyTemplateImpl::FilterView(const FString& InFilterText)
{
	TArray<FString> FilterStrings;

	FString ParseString = InFilterText;
	// Remove whitespace from the front and back of the string
	ParseString.TrimStartAndEndInline();
	ParseString.ParseIntoArray(FilterStrings, TEXT(" "), true);

	RootPropertyNode->FilterNodes(FilterStrings);
	RootPropertyNode->ProcessSeenFlags(true);

	bHasActiveFilter = FilterStrings.Num() > 0;

	if (!bNodeTreeExternallyManaged)
	{
		UpdateTopLevelPropertyNodes(FPropertyNode::FindPropertyNodeByPath(RootPath, RootPropertyNode.ToSharedRef()));
	}

	RequestRefresh();
}


void SPropertyTemplateImpl::OnGetChildrenForPropertyNode(TSharedPtr<FPropertyNode> InPropertyNode, TArray< TSharedPtr<FPropertyNode> >& OutChildren)
{
	if (CurrentFilterText.Len() > 0)
	{
		if (InPropertyNode->HasNodeFlags(EPropertyNodeFlags::IsSeenDueToChildFiltering))
		{
			// The node should be expanded because its children are in the filter
			RequestItemExpanded(InPropertyNode, true);
		}
		else if (InPropertyNode->HasNodeFlags(EPropertyNodeFlags::AutoExpanded))
		{
			// This property node has no children in the filter and was previously auto expanded
			// So collapse it now
			InPropertyNode->SetNodeFlags(EPropertyNodeFlags::AutoExpanded, false);
			RequestItemExpanded(InPropertyNode, false);
		}
	}
	else
	{
		// Check and see if the node wants to be expanded and we haven't already expanded this node before
		if (InPropertyNode->HasNodeFlags(EPropertyNodeFlags::Expanded) != 0 && InPropertyNode->HasNodeFlags(EPropertyNodeFlags::HasEverBeenExpanded) == 0)
		{
			RequestItemExpanded(InPropertyNode, true);
		}

		// No nodes are auto expanded when we have no filter 
		InPropertyNode->SetNodeFlags(EPropertyNodeFlags::AutoExpanded, false);
	}

	// If we are getting children for this node then its been expanded
	InPropertyNode->SetNodeFlags(EPropertyNodeFlags::HasEverBeenExpanded, true);

	for (int32 ChildIndex = 0; ChildIndex < InPropertyNode->GetNumChildNodes(); ++ChildIndex)
	{
		TSharedPtr<FPropertyNode> ChildNode = InPropertyNode->GetChildNode(ChildIndex);
		FObjectPropertyNode* ObjNode = ChildNode->AsObjectNode();

		bool bPropertyVisible = true;
		UProperty* Property = ChildNode->GetProperty();
		if (Property != NULL && IsPropertyVisible.IsBound())
		{
			TArray< TWeakObjectPtr<UObject> > Objects;
			if (ObjNode)
			{
				for (int32 ObjectIndex = 0; ObjectIndex < ObjNode->GetNumObjects(); ++ObjectIndex)
				{
					Objects.Add(ObjNode->GetUObject(ObjectIndex));
				}
			}

			FPropertyAndParent PropertyAndParent(*Property, InPropertyNode->GetProperty(), Objects);

			bPropertyVisible = IsPropertyVisible.Execute(PropertyAndParent);
		}

		if (bPropertyVisible)
		{
			if (ObjNode)
			{
				// Currently object property nodes do not provide any useful information other than being a container for its children.  We do not draw anything for them.
				// When we encounter object property nodes, add their children instead of adding them to the tree.
				OnGetChildrenForPropertyNode(ChildNode, OutChildren);
			}
			else
			{
				if (ChildNode->IsVisible())
				{
					// Don't add empty category nodes
					if (ChildNode->AsCategoryNode() == NULL || ChildNode->GetNumChildNodes() > 0)
					{
						OutChildren.Add(ChildNode);
					}
				}
			}
		}
	}
}

TSharedRef<ITableRow> SPropertyTemplateImpl::OnGenerateRowForPropertyTree(TSharedPtr<FPropertyNode> InPropertyNode, const TSharedRef<STableViewBase>& OwnerTable)
{
	// Generate a row that represents a property
	return CreatePropertyEditor(InPropertyNode, OwnerTable);
}

void SPropertyTemplateImpl::OnSelectionChanged(TSharedPtr<FPropertyNode> InPropertyNode, ESelectInfo::Type SelectInfo)
{
	if (InPropertyNode.IsValid())
	{
		OnPropertySelectionChanged.ExecuteIfBound(InPropertyNode->GetProperty());
	}
}


void SPropertyTemplateImpl::RequestItemExpanded(TSharedPtr<FPropertyNode> PropertyNode, bool bExpand, bool bRecursiveExpansion )
{
	// Don't change expansion state if its already in that state
	if (PropertyTree->IsItemExpanded(PropertyNode) != bExpand)
	{
		PropertyNode->SetNodeFlags(EPropertyNodeFlags::AutoExpanded, true);
		FilteredNodesRequestingExpansionState.Add(PropertyNode, bExpand);
	}

	if (bRecursiveExpansion)
	{
		check(PropertyNode.IsValid());
		int32 NumChildren = PropertyNode->GetNumChildNodes();
		for (int32 Index = 0; Index < NumChildren; ++Index)
		{
			TSharedPtr<FPropertyNode> ChildNode = PropertyNode->GetChildNode(Index);
			if (ChildNode.IsValid())
			{
				RequestItemExpanded(ChildNode, bExpand, bRecursiveExpansion);
			}
		}
	}
}

/** Updates the top level property nodes.  The root nodes for the treeview. */
void SPropertyTemplateImpl::UpdateTopLevelPropertyNodes(TSharedPtr<FPropertyNode> FirstVisibleNode)
{
	TopLevelPropertyNodes.Empty();

	if (FirstVisibleNode.IsValid())
	{
		FObjectPropertyNode* ObjNode = FirstVisibleNode->AsObjectNode();
		if (ObjNode || !bShowTopLevelPropertyNodes)
		{
			// Currently object property nodes do not provide any useful information other than being a container for its children.  We do not draw anything for them.
			// When we encounter object property nodes, add their children instead of adding them to the tree.
			OnGetChildrenForPropertyNode(FirstVisibleNode, TopLevelPropertyNodes);
		}
		else if (bShowTopLevelPropertyNodes)
		{
			TopLevelPropertyNodes.Add(FirstVisibleNode);
		}
	}
}


void SPropertyTemplateImpl::CreateColorPickerWindow(const TSharedRef< class FPropertyEditor >& PropertyEditor, bool bUseAlpha)
{
	const TSharedRef< FPropertyNode > PropertyNode = PropertyEditor->GetPropertyNode();
	ColorPropertyNode = &PropertyNode.Get();

	check(ColorPropertyNode);
	UProperty* Property = ColorPropertyNode->GetProperty();
	check(Property);

	FReadAddressList ReadAddresses;
	ColorPropertyNode->GetReadAddress(false, ReadAddresses, false);

	TArray<FLinearColor*> LinearColor;
	TArray<FColor*> DWORDColor;
	if (ReadAddresses.Num())
	{
		const uint8* Addr = ReadAddresses.GetAddress(0);
		if (Addr)
		{
			if (Cast<UStructProperty>(Property)->Struct->GetFName() == NAME_Color)
			{
				DWORDColor.Add((FColor*)Addr);
			}
			else
			{
				check(Cast<UStructProperty>(Property)->Struct->GetFName() == NAME_LinearColor);
				LinearColor.Add((FLinearColor*)Addr);
			}
		}
	}

	if (DWORDColor.Num() || LinearColor.Num())
	{
		FColorPickerArgs PickerArgs;
		PickerArgs.ParentWidget = AsShared();
		PickerArgs.bUseAlpha = bUseAlpha;
		PickerArgs.DisplayGamma = TAttribute<float>::Create(TAttribute<float>::FGetter::CreateUObject(GEngine, &UEngine::GetDisplayGamma));
		PickerArgs.ColorArray = &DWORDColor;
		PickerArgs.LinearColorArray = &LinearColor;
		PickerArgs.OnColorCommitted = FOnLinearColorValueChanged::CreateSP(this, &SPropertyTemplateImpl::SetColor);

		OpenColorPicker(PickerArgs);
	}
}


void SPropertyTemplateImpl::EnqueueDeferredAction(FSimpleDelegate& DeferredAction)
{
	DeferredActions.Add(DeferredAction);
}

void SPropertyTemplateImpl::RequestRefresh()
{
	PropertyTree->RequestTreeRefresh();
}


void SPropertyTemplateImpl::SetFilterText(const FText& InFilterText)
{
	FilterTextBox->SetText(InFilterText);
}

TSharedRef<ITableRow> SPropertyTemplateImpl::CreatePropertyEditor(TSharedPtr<FPropertyNode> InPropertyNode, const TSharedPtr<STableViewBase>& OwnerTable)
{
	FCategoryPropertyNode* CategoryNode = InPropertyNode->AsCategoryNode();

	if (CategoryNode != nullptr)
	{
		// This is a category node; it does not need columns.
		// Just use a simple setup.
		return SNew(SPropertyTreeCategoryRow, OwnerTable.ToSharedRef())
			.DisplayName(CategoryNode->GetDisplayName());
	}
	else
	{
		TSharedRef< IPropertyUtilities > PropertyUtilities = PropertySettings.ToSharedRef();
		TSharedRef< FPropertyEditor > PropertyEditor = FPropertyEditor::Create(InPropertyNode.ToSharedRef(), PropertyUtilities);
		return SNew(SPropertyEditorTableRow, PropertyEditor, PropertyUtilities, OwnerTable.ToSharedRef())
			.OnMiddleClicked(OnPropertyMiddleClicked);
	}
}

void SPropertyTemplateImpl::SetColor(FLinearColor NewColor)
{
	check(ColorPropertyNode);
	UProperty* NodeProperty = ColorPropertyNode->GetProperty();
	check(NodeProperty);
	FObjectPropertyNode* ObjectNode = ColorPropertyNode->FindObjectItemParent();

	// If more than one object is selected, an empty field indicates their values for this property differ.
	// Don't send it to the objects value in this case (if we did, they would all get set to None which isn't good).
	if (ObjectNode && ObjectNode->GetNumObjects() == 1)
	{
		FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "SetColorProperty", "Set Color Property"));

		ColorPropertyNode->NotifyPreChange(NodeProperty, GetNotifyHook());

		FPropertyChangedEvent ChangeEvent(NodeProperty, EPropertyChangeType::ValueSet);
		ColorPropertyNode->NotifyPostChange(ChangeEvent, GetNotifyHook());
	}
}


void SPropertyTemplateImpl::SetObjectArray(const TArray< TWeakObjectPtr< UObject > >& InObjects)
{
	check(RootPropertyNode.IsValid());

	PreSetObject();

	bool bOwnedByLockedLevel = false;
	for (int32 ObjectIndex = 0; ObjectIndex < InObjects.Num(); ++ObjectIndex)
	{
		RootPropertyNode->AddObject(InObjects[ObjectIndex].Get());
	}

	// @todo Slate Property Window
	//SetFlags(EPropertyWindowFlags::ReadOnly, bOwnedByLockedLevel);

	PostSetObject();

	// Set the title of the window based on the objects we are viewing
	if (!RootPropertyNode->GetObjectBaseClass())
	{
		Title = NSLOCTEXT("PropertyView", "NothingSelectedTitle", "Nothing selected").ToString();
	}
	else if (RootPropertyNode->GetNumObjects() == 1)
	{
		// if the object is the default metaobject for a UClass, use the UClass's name instead
		const UObject* Object = RootPropertyNode->ObjectConstIterator()->Get();
		FString ObjectName = Object->GetName();
		if (Object->GetClass()->GetDefaultObject() == Object)
		{
			ObjectName = Object->GetClass()->GetName();
		}
		else
		{
			// Is this an actor?  If so, it might have a friendly name to display
			const AActor* Actor = Cast<const  AActor >(Object);
			if (Actor != NULL && !Object->IsTemplate())
			{
				// Use the friendly label for this actor
				ObjectName = Actor->GetActorLabel();
			}
		}

		Title = ObjectName;
	}
	else
	{
		Title = FText::Format(NSLOCTEXT("PropertyView", "MultipleSelectedFmt", "{0} ({1} selected)"), FText::FromString(RootPropertyNode->GetObjectBaseClass()->GetName()), RootPropertyNode->GetNumObjects()).ToString();
	}

	OnObjectArrayChanged.ExecuteIfBound(Title, InObjects);
}

/** Called before during SetObjectArray before we change the objects being observed */
void SPropertyTemplateImpl::PreSetObject()
{
	check(RootPropertyNode.IsValid());

	// Save all expanded items before setting new objects 
	SaveExpandedItems();

	// Save all the column widths before setting new objects
	SaveColumnWidths();

	RootPropertyNode->RemoveAllObjects();
}


void SPropertyTemplateImpl::SaveExpandedItems()
{
	if (RootPropertyNode->GetNumChildNodes() > 0)
	{
		TSet< TSharedPtr<FPropertyNode> > ExpandedNodes;
		PropertyTree->GetExpandedItems(ExpandedNodes);

		TArray<FString> ExpandedItemNames;

		for (TSet< TSharedPtr<FPropertyNode> >::TConstIterator It(ExpandedNodes); It; ++It)
		{
			TSharedPtr<FPropertyNode> PropertyNode = *It;

			//don't save the root, it gets expanded by default
			if (PropertyNode->GetParentNode())
			{
				const bool bWithArrayIndex = true;
				FString Path;
				Path.Empty(128);
				PropertyNode->GetQualifiedName(Path, bWithArrayIndex);

				new(ExpandedItemNames)FString(Path);
			}
		}


		UClass* BestBaseClass = RootPropertyNode->GetObjectBaseClass();
		//while a valid class, and we're either the same as the base class (for multiple actors being selected and base class is AActor) OR we're not down to AActor yet)
		for (UClass* Class = BestBaseClass; Class && ((BestBaseClass == Class) || (Class != AActor::StaticClass())); Class = Class->GetSuperClass())
		{
			FString ExpansionName = Class->GetName();
			//			@todo Slate Property window
			// 			if (HasFlags(EPropertyWindowFlags::Favorites))
			// 			{
			// 				ExpansionName += TEXT("Favorites");
			// 			}

			//GConfig->SetSingleLineArray(TEXT("PropertyWindowExpansion"), *ExpansionName, ExpandedItemNames, GEditorPerProjectIni);
		}
	}
}

void SPropertyTemplateImpl::SaveColumnWidths()
{
	const TIndirectArray<SHeaderRow::FColumn>& Columns = ColumnHeaderRow->GetColumns();
	for (int32 Idx = 0; Idx < Columns.Num(); ++Idx)
	{
		const SHeaderRow::FColumn& Column = Columns[Idx];
		const float Width = Column.GetWidth();
		//GConfig->SetFloat(TEXT("PropertyWindowWidths"), *Column.ColumnId.ToString(), Width, GEditorPerProjectIni);
	}
}


void SPropertyTemplateImpl::PostSetObject()
{
	check(RootPropertyNode.IsValid());
	check(!bNodeTreeExternallyManaged);

	DestroyColorPicker();
	ColorPropertyNode = NULL;

	// Reconstruct the property tree so we don't have a tree filled with data we are about to destroy
	ConstructPropertyTree();

	FPropertyNodeInitParams InitParams;
	InitParams.ParentNode = NULL;
	InitParams.Property = NULL;
	InitParams.ArrayOffset = 0;
	InitParams.ArrayIndex = INDEX_NONE;
	InitParams.bAllowChildren = true;
	InitParams.bForceHiddenPropertyVisibility = bForceHiddenPropertyVisibility;

	RootPropertyNode->InitNode(InitParams);

	RootPropertyNode->ProcessSeenFlags(true);
	UpdateTopLevelPropertyNodes(FPropertyNode::FindPropertyNodeByPath(RootPath, RootPropertyNode.ToSharedRef()));


	//// Restore expansion state of items in the tree
	//RestoreExpandedItems();

	//// Restore the widths of columns
	//RestoreColumnWidths();

	RequestRefresh();
}


void SPropertyTemplateImpl::SetRootPath(const TSharedPtr< FPropertyPath >& Path)
{
	if (Path.IsValid())
	{
		RootPath = Path.ToSharedRef();
	}
	else
	{
		RootPath = FPropertyPath::CreateEmpty();
	}

	ConstructPropertyTree();
	UpdateTopLevelPropertyNodes(FPropertyNode::FindPropertyNodeByPath(RootPath, RootPropertyNode.ToSharedRef()));

	//// Restore expansion state of items in the tree
	//RestoreExpandedItems();

	//// Restore the widths of columns
	//RestoreColumnWidths();
}

TSharedRef< FPropertyPath > SPropertyTemplateImpl::GetRootPath() const
{
	return RootPath;
}