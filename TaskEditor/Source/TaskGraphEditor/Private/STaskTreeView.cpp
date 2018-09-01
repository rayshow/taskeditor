#include"STaskTreeView.h"
#include"GenericCommands.h"
#include"MultiBoxExtender.h"
#include"MultiBoxBuilder.h"
#include"EditorStyle.h"
#include"TaskSystem.h"
#include"SharedPointer.h"

#define LOCTEXT_NAMESPACE "TaskGraphEditor"

void AddChildrenNode(TSharedPtr<FTaskTreeItem> Node)
{
	if (Node.IsValid())
	{
		UTaskGroup* Group = Cast<UTaskGroup>(Node->Object);
		if (Group)
		{
			for (auto Child : Group->Children)
			{
				TSharedPtr<FTaskTreeItem> newItem = MakeShareable(new FTaskTreeItem(Node, true, Child));
				if (Child->GetClass()->IsChildOf<UTaskGroup>())
				{
					AddChildrenNode(newItem);
				}
				Node->Children.Add(newItem);
			}
		}
	}
}

void FTaskTreeItem::RebuildChildrenItem()
{
	UTaskGroup* Group = Cast<UTaskGroup>(Object);
	if (Group)
	{
		for (auto Child : Group->Children)
		{
			UTaskGroup* ChildObject = Cast<UTaskGroup>(Child);
			TSharedPtr<FTaskTreeItem> newItem = MakeShareable(
				new FTaskTreeItem(SharedThis(this), true, Child));
			Children.Add(newItem);

			AddChildrenNode(newItem);
		}
	}
}

FTaskTreeItem::FTaskTreeItem( TSharedPtr<FTaskTreeItem> InParent,
	bool Group, UTaskObject* InObject)
	: Object(nullptr), Parent(MoveTemp(InParent)),IsGroup{ Group } 
{
	//build From Exists Data
	if (InObject)
	{
		Object = InObject;
	}
	// Item Add From TaskTree Panel
	else {
		// is Group Node
		if (IsGroup)
		{
			//Is TaskTree Mid Node, Has Parent Node
			if (InParent.IsValid())
			{
				Object = NewObject<UTaskGroup>(InParent->Object);
				InParent->Object->AddChild(Object);
			}
			//Is Root Node, No Parent
			else {
				Object = NewObject<UTaskModule>();
			}
		}
		// Is Task leaf Node, InParent must exists
		else {
			check(InParent.IsValid());
			Object = NewObject<UTask>(InParent->Object);
			InParent->Object->AddChild(Object);
		}	
	}
	if (Object) {
		Object->Initialize();
	}
}


TSharedPtr<FTaskTreeItem> MakeChildItem(TSharedPtr<FTaskTreeItem> Parent,
	bool IsGroup = false, UTaskObject* TaskParent = nullptr)
{
	TSharedPtr<FTaskTreeItem> Task = MakeShareable(new FTaskTreeItem(Parent, IsGroup, TaskParent));
	if (Parent.IsValid())
		Parent->Children.Add(Task);
	return Task;
}

void FTaskTreeItem::SetName(FName InName)
{
	if (Object) {
		Object->Name = InName;
	}
}

bool FTaskTreeItem::IsChildExists(FName const& Name)
{
	for (auto& child : Children) {
		if (child->Object &&
			child->Object->Name == Name) {
			return true;
		}
	}
	return false;
}

bool FTaskTreeItem::IsChildOf(const FTaskTreeItem& InParent)
{
	auto CurrentParent = Parent.Pin();
	while (CurrentParent.IsValid())
	{
		if (CurrentParent.Get() == &InParent)
		{
			return true;
		}
		CurrentParent = CurrentParent->Parent.Pin();
	}
	return false;
}



void STaskTreeView::BindCommand()
{
	ContextMenuCommands = TSharedPtr< FUICommandList >(new FUICommandList);

	ContextMenuCommands->MapAction(FGenericCommands::Get().Rename, FUIAction(
		FExecuteAction::CreateSP(this, &STaskTreeView::HandleRenameCommand),
		FCanExecuteAction::CreateSP(this, &STaskTreeView::CanRenameCommandExecute)
	));

	ContextMenuCommands->MapAction(FGenericCommands::Get().Delete, FUIAction(
		FExecuteAction::CreateSP(this, &STaskTreeView::HandleDeleteCommand),
		FCanExecuteAction::CreateSP(this, &STaskTreeView::CanDeleteCommandExecute)
	));
}

void STaskTreeView::AddTaskModule(UTaskModule* Module)
{
	TSharedPtr<FTaskTreeItem> Root = MakeShareable(new FTaskTreeItem(nullptr, true, Module));
	if (Root->Children.Num() == 0 ) {
		Root->RebuildChildrenItem();
	}
	TreeRootItems.Add(Root);
}

void STaskTreeView::Construct(const FArguments& InArgs)
{
	OnSingleItemSelected = InArgs._OnSingleItemSelected;

	SAssignNew(TreeViewPtr, STreeView< TSharedPtr<FTaskTreeItem> >)
		.TreeItemsSource(&TreeRootItems)
		.OnGenerateRow(this, &STaskTreeView::GenerateTreeRow)
		.OnItemScrolledIntoView(this, &STaskTreeView::TreeItemScrolledIntoView)
		.ItemHeight(18)
		.SelectionMode(InArgs._SelectionMode)
		.OnSelectionChanged(this, &STaskTreeView::TreeSelectionChanged)
		.OnExpansionChanged(this, &STaskTreeView::TreeExpansionChanged)
		.OnGetChildren(this, &STaskTreeView::GetChildrenForTree)
		.OnSetExpansionRecursive(this, &STaskTreeView::SetTreeItemExpansionRecursive)
		.OnContextMenuOpening(this, &STaskTreeView::MakeViewContextMenu)
		.ClearSelectionOnClick(true);

	BindCommand();

	ChildSlot
		[
			SNew(SVerticalBox)

			//		// Search
			//	+ SVerticalBox::Slot()
			//	.AutoHeight()
			//	.Padding(0, 1, 0, 3)
			//	[
			//		SNew(SHorizontalBox)

			//		+ SHorizontalBox::Slot()
			//	.AutoWidth()
			//	[
			//		InArgs._SearchContent.Widget
			//	]

			//+ SHorizontalBox::Slot()
			//	.FillWidth(1.0f)
			//	[
			//		SAssignNew(SearchBoxPtr, SSearchBox)
			//		.Visibility(InArgs._SearchBarVisibility)
			//		.HintText(LOCTEXT("AssetTreeSearchBoxHint", "Search Folders"))
			//		.OnTextChanged(this, &SPathView::OnAssetTreeSearchBoxChanged)
			//		.OnTextCommitted(this, &SPathView::OnAssetTreeSearchBoxCommitted)
			//		]
			//	]

			//// Tree title
			//+ SVerticalBox::Slot()
			//	.AutoHeight()
			//	[
			//		SNew(STextBlock)
			//		.Font(FEditorStyle::GetFontStyle("ContentBrowser.SourceTitleFont"))
			//		.Text(this, &SPathView::GetTreeTitle)
			//		.Visibility(InArgs._ShowTreeTitle ? EVisibility::Visible : EVisibility::Collapsed)
			//	]

			//// Separator
			//+ SVerticalBox::Slot()
			//	.AutoHeight()
			//	.Padding(0, 0, 0, 1)
			//	[
			//		SNew(SSeparator)
			//		.Visibility((InArgs._ShowSeparator) ? EVisibility::Visible : EVisibility::Collapsed)
			//	]

			// Tree
			+SVerticalBox::Slot()
			.FillHeight(1.f)
			[
				TreeViewPtr.ToSharedRef()
			]
		];

	//TreeViewPtr->SetItemExpansion(Root, true);
	 
}

STaskTreeView::~STaskTreeView()
{

}

bool STaskTreeView::IsTreeItemExpanded(TSharedPtr<FTaskTreeItem> TreeItem) const
{
	return TreeViewPtr->IsItemExpanded(TreeItem);
}

TSharedRef<ITableRow> STaskTreeView::GenerateTreeRow(
	TSharedPtr<FTaskTreeItem> TreeItem, const TSharedRef<STableViewBase>& OwnerTable)
{
	check(TreeItem.IsValid());

	return
		SNew(STableRow< TSharedPtr<FTaskTreeItem> >, OwnerTable)
		.OnDragDetected(this, &STaskTreeView::OnFolderDragDetected)
		[
			SNew(STaskTreeItem)
			.TreeItem(TreeItem)
			.IsItemExpanded(this, &STaskTreeView::IsTreeItemExpanded, TreeItem)
		/*.OnNameChanged(this, &STaskTreeView::FolderNameChanged)
		.OnVerifyNameChanged(this, &STaskTreeView::VerifyFolderNameChanged)
		.OnAssetsOrPathsDragDropped(this, &STaskTreeView::TreeAssetsOrPathsDropped)
		.OnFilesDragDropped(this, &STaskTreeView::TreeFilesDropped)

		.HighlightText(this, &STaskTreeView::GetHighlightText)
		.IsSelected(this, &STaskTreeView::IsTreeItemSelected, TreeItem)*/
		];
}

void STaskTreeView::TreeItemScrolledIntoView(
	TSharedPtr<FTaskTreeItem> TreeItem, const TSharedPtr<ITableRow>& Widget)
{
	if (Widget.IsValid() && Widget->GetContent().IsValid())
	{
		TreeItem->OnRenamedRequestEvent.Broadcast();
	}
}

void STaskTreeView::TreeSelectionChanged(TSharedPtr< FTaskTreeItem > TreeItem, ESelectInfo::Type SelectInfo)
{
	if (true)
	{
		const TArray<TSharedPtr<FTaskTreeItem>> SelectedItems = TreeViewPtr->GetSelectedItems();

		//notify change graph
		if (SelectedItems.Num() == 1) {
			OnSingleItemSelected.ExecuteIfBound(SelectedItems[0]->Object);
		}
		else if (SelectedItems.Num() == 0) {
			OnSingleItemSelected.ExecuteIfBound(nullptr);
		}

		for (int32 ItemIdx = 0; ItemIdx < SelectedItems.Num(); ++ItemIdx)
		{
			const TSharedPtr<FTaskTreeItem> Item = SelectedItems[ItemIdx];
			if (!ensure(Item.IsValid()))
			{
				continue;
			}
		}
	}
}

void STaskTreeView::TreeExpansionChanged(TSharedPtr< FTaskTreeItem > TreeItem, bool bIsExpanded)
{
	if (true)
	{
		TSet<TSharedPtr<FTaskTreeItem>> ExpandedItemSet;
		TreeViewPtr->GetExpandedItems(ExpandedItemSet);
		const TArray<TSharedPtr<FTaskTreeItem>> ExpandedItems = ExpandedItemSet.Array();

		for (int32 ItemIdx = 0; ItemIdx < ExpandedItems.Num(); ++ItemIdx)
		{
			const TSharedPtr<FTaskTreeItem> Item = ExpandedItems[ItemIdx];
			if (!ensure(Item.IsValid()))
			{
				// All items must exist
				continue;
			}
		}
	}
}

void STaskTreeView::GetChildrenForTree(TSharedPtr< FTaskTreeItem > TreeItem,
	TArray< TSharedPtr<FTaskTreeItem> >& OutChildren)
{
	OutChildren = TreeItem->Children;
}

void STaskTreeView::SetTreeItemExpansionRecursive(TSharedPtr< FTaskTreeItem > TreeItem, bool bInExpansionState)
{
	TreeViewPtr->SetItemExpansion(TreeItem, bInExpansionState);

	// Recursively go through the children.
	for (auto It = TreeItem->Children.CreateIterator(); It; ++It)
	{
		SetTreeItemExpansionRecursive(*It, bInExpansionState);
	}
}

void STaskTreeView::HandleCreateNewTaskGroup()
{
	auto Items = TreeViewPtr->GetSelectedItems();

	if (Items.Num() == 1)
	{
		auto& Parent = Items[0];

		// Create a valid base name for this folder
		FName DefaultTaskBaseName = TEXT("新任务组");
		FName TaskItemName = DefaultTaskBaseName;
		int32 NewFolderPostfix = 1;

		while (Parent->IsChildExists(TaskItemName))
		{
			FString Form = FString::Printf(TEXT("新任务组%d"), NewFolderPostfix);
			TaskItemName = FName(*Form);
			NewFolderPostfix++;
		}
		//InOnCreateNewFolder.ExecuteIfBound(DefaultFolderName.ToString(), FolderPath);

		//Create Item
		TSharedPtr<FTaskTreeItem> newGroup = MakeChildItem(Parent, true);
		newGroup->SetName(TaskItemName);

		//Select and Refresh
		TreeViewPtr->SetSelection(newGroup);
		this->SetTreeItemExpansionRecursive(Parent, true);
		TreeViewPtr->RequestTreeRefresh();
		TreeViewPtr->RequestScrollIntoView(newGroup);
	}
}

void STaskTreeView::HandleCreateNewTask()
{
	auto Items = TreeViewPtr->GetSelectedItems();

	if (Items.Num() == 1)
	{
		auto& Parent = Items[0];

		// Create a valid base name for this folder
		FName DefaultTaskBaseName = TEXT("新任务");
		FName TaskItemName = DefaultTaskBaseName;
		int32 NewFolderPostfix = 1;

		while (Parent->IsChildExists(TaskItemName) )
		{
			FString Form = FString::Printf(TEXT("新任务%d"), NewFolderPostfix);
			TaskItemName = FName(*Form);
			NewFolderPostfix++;
		}
		//InOnCreateNewFolder.ExecuteIfBound(DefaultFolderName.ToString(), FolderPath);

		//Create Item
		TSharedPtr<FTaskTreeItem> newItem = MakeChildItem(Parent);
		newItem->SetName(TaskItemName);
		
		//Select and Refresh
		TreeViewPtr->SetSelection(newItem);
		this->SetTreeItemExpansionRecursive(Parent, true);
		TreeViewPtr->RequestTreeRefresh();
		TreeViewPtr->RequestScrollIntoView(newItem);
	}	
}

void STaskTreeView::HandleDeleteTask()
{
	auto Items = TreeViewPtr->GetSelectedItems();
	if (Items.Num() == 1)
	{
		auto& Curr = Items[0];
		auto Parent = Curr->Parent.Pin();
		if (Parent.IsValid()) {
			Parent->Children.Remove(Curr);
			TreeViewPtr->RequestTreeRefresh();
		}
	}
}


void STaskTreeView::HandleRenameCommand()
{

}

void STaskTreeView::HandleDeleteCommand()
{

}

bool STaskTreeView::CanRenameCommandExecute()
{
	return true;
}


bool STaskTreeView::CanDeleteCommandExecute()
{
	return true;
}


bool STaskTreeView::CanCreateNewTaskGroup()
{
	auto Items = TreeViewPtr->GetSelectedItems();
	return Items.Num()==1 &&
		Items[0]->Object->GetClass()->IsChildOf<UTaskGroup>();
}

bool STaskTreeView::CanCreateNewTask()
{
	auto Items = TreeViewPtr->GetSelectedItems();
	return Items.Num() == 1 &&
		Items[0]->Object->GetClass()->IsChildOf<UTaskGroup>() &&
		!Items[0]->Object->GetClass()->IsChildOf<UTaskModule>();
}

bool STaskTreeView::CanDeleteItems()
{
	auto Items = TreeViewPtr->GetSelectedItems();
	if (Items.Num() <= 0) return false;
	for (auto& Item : Items) {
		//Can't Delete TaskModule Directly
		if (Item->Object->GetClass()->IsChildOf<UTaskModule>())
			return false;
	}
	return true;
}


TSharedPtr<SWidget> STaskTreeView::MakeViewContextMenu()
{
	auto Items = TreeViewPtr->GetSelectedItems();
	if (Items.Num() <= 0)
	{
		return nullptr;
	}

	FMenuBuilder MenuBuilder(true, ContextMenuCommands, nullptr, true);

	// Create-Ops Only On Single Selected Item, Only Group Can Create Sub-Group or Sub-Task

	{
		MenuBuilder.AddMenuEntry(
			LOCTEXT("新任务组", "新任务组"),
			LOCTEXT("新任务组tip", "新任务组tip"),
			FSlateIcon(FEditorStyle::GetStyleSetName(),
				"ContentBrowser.NewFolderIcon"),
			FUIAction(
				FExecuteAction::CreateSP(this, &STaskTreeView::HandleCreateNewTaskGroup),
				FCanExecuteAction::CreateSP(this, &STaskTreeView::CanCreateNewTaskGroup)
			), TEXT("新任务组"));

		MenuBuilder.AddMenuEntry(
			LOCTEXT("新任务", "新任务"),
			LOCTEXT("新任务tip", "新任务tip"),
			FSlateIcon(FEditorStyle::GetStyleSetName(),
				"ContentBrowser.NewFolderIcon"),
			FUIAction(
				FExecuteAction::CreateSP(this, &STaskTreeView::HandleCreateNewTask),
				FCanExecuteAction::CreateSP(this, &STaskTreeView::CanCreateNewTask)
			), TEXT("新任务"));
	}
	
	
	MenuBuilder.AddMenuEntry(
		LOCTEXT("删除", "删除"),
		LOCTEXT("删除tip", "删除tip"),
		FSlateIcon(FEditorStyle::GetStyleSetName(),
			"ContentBrowser.NewFolderIcon"),
		FUIAction(
			FExecuteAction::CreateSP(this, &STaskTreeView::HandleDeleteTask),
			FCanExecuteAction::CreateSP(this, &STaskTreeView::CanDeleteItems)
		), TEXT("删除"));


	return MenuBuilder.MakeWidget();
}


FReply STaskTreeView::OnFolderDragDetected(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		TArray<TSharedPtr<FTaskTreeItem>> SelectedItems = TreeViewPtr->GetSelectedItems();
		if (SelectedItems.Num())
		{
			return FReply::Handled();// .BeginDragDrop(FAssetDragDropOp::New(PathNames));
		}
	}

	return FReply::Unhandled();
}

#undef LOCTEXT_NAMESPACE 