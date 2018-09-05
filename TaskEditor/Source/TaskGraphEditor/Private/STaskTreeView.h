#pragma once

#include"TreeView.h"
#include"SharedPointer.h"
#include"EditableTextBox.h"
#include"SInlineEditableTextBlock.h"
#include"UICommandList.h"

class UTaskObject;

// represent TaskTree Item data struct
class FTaskTreeItem 
	:public TSharedFromThis<FTaskTreeItem>
{
public:
	UTaskObject* Object;
	bool         IsGroup;
	
	TArray< TSharedPtr<FTaskTreeItem>> Children;
	TWeakPtr< FTaskTreeItem> Parent;

	DECLARE_MULTICAST_DELEGATE(FRenamedRequestEvent)
	FRenamedRequestEvent OnRenamedRequestEvent;

	FTaskTreeItem(TSharedPtr<FTaskTreeItem> ItemParent,
		bool Group = false, UTaskObject* InObject = nullptr);

	void RebuildChildrenItem();

	void SetName(FName Name);
	
	bool IsChildExists(FName const& Name);

	bool IsChildOf(const FTaskTreeItem& InParent);
};


class STaskTreeItem : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(STaskTreeItem)
		: _TreeItem(TSharedPtr<FTaskTreeItem>())
	{}
		SLATE_ARGUMENT(TSharedPtr<FTaskTreeItem>, TreeItem)
		/** True when this item has children and is expanded */
		SLATE_ATTRIBUTE(bool, IsItemExpanded)

	SLATE_END_ARGS();

	~STaskTreeItem();

	void Construct(const FArguments& InArgs);

	bool ValidateDragDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent, bool& OutIsKnownDragOperation) const;
	virtual void OnDragEnter(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
	virtual void OnDragLeave(const FDragDropEvent& DragDropEvent) override;
	virtual FReply OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
	virtual FReply OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	const  FSlateBrush* GetBorderImage() const;
	FText GetNameText() const;
	bool VerifyNameChanged(const FText& InName, FText& OutError) const { return true; }
	void HandleNameCommitted(const FText& NewText, ETextCommit::Type /*CommitInfo*/);
	TWeakPtr<FTaskTreeItem>                TreeItem;
	TSharedPtr<SEditableTextBox>           EditableName;
	TAttribute<bool>                       IsItemExpanded;
	TSharedPtr< SInlineEditableTextBlock > InlineRenameWidget;
	FDelegateHandle EnterEditingModeDelegateHandle;
	bool bDraggedOver;
};


DECLARE_DELEGATE_RetVal(TSharedPtr<SWidget>, FOnGetTaskTreeContextMenu);
DECLARE_DELEGATE_OneParam(FOnSingleItemSelected, UTaskObject*);

class STaskTreeView : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(STaskTreeView) {}
		SLATE_ARGUMENT(ESelectionMode::Type, SelectionMode)
	    SLATE_EVENT(FOnSingleItemSelected, OnSingleItemSelected)
	SLATE_END_ARGS()

	~STaskTreeView();

	void Construct(const FArguments& InArgs);
	void BindCommand();

	//Handle Event CallBack
	void HandleRenameCommand();
	void HandleDeleteCommand();
	void HandleCreateNewTaskGroup();
	void HandleCreateNewTask();
	void HandleDeleteTask();


	//Judge of Command Execuse
	bool CanRenameCommandExecute();
	bool CanDeleteCommandExecute();
	bool CanCreateNewTaskGroup();
	bool CanCreateNewTask();
	bool CanDeleteItems();

	// add Module to 
	void AddTaskModule(UTaskModule* Module);

	virtual TSharedRef<ITableRow> GenerateTreeRow(
		TSharedPtr<FTaskTreeItem> TreeItem, const TSharedRef<STableViewBase>& OwnerTable);

	void TreeItemScrolledIntoView(TSharedPtr<FTaskTreeItem> TreeItem, const TSharedPtr<ITableRow>& Widget);
	void TreeSelectionChanged(TSharedPtr< FTaskTreeItem > TreeItem, ESelectInfo::Type SelectInfo);
	void TreeExpansionChanged(TSharedPtr< FTaskTreeItem > TreeItem, bool bIsExpanded);
	void GetChildrenForTree(TSharedPtr< FTaskTreeItem > TreeItem, TArray< TSharedPtr<FTaskTreeItem> >& OutChildren);
	void SetTreeItemExpansionRecursive(TSharedPtr< FTaskTreeItem > TreeItem, bool bInExpansionState);
	TSharedPtr<SWidget> MakeViewContextMenu();
	bool IsTreeItemExpanded(TSharedPtr<FTaskTreeItem> TreeItem) const;

	FReply OnFolderDragDetected(const FGeometry& Geometry, const FPointerEvent& MouseEvent);


	TSharedPtr< STreeView< TSharedPtr<FTaskTreeItem>> > TreeViewPtr;
	TArray< TSharedPtr<FTaskTreeItem> > TreeRootItems;
	TArray< UTaskModule*>               Modules;
	FOnSingleItemSelected               OnSingleItemSelected;

	FOnGetTaskTreeContextMenu           OnMainContentMenu;
	TSharedPtr<FUICommandList>          ContextMenuCommands;
};