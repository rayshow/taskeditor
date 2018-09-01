#include"STaskTreeView.h"
#include"EditorStyle.h"
#include"TaskSystem.h"

const FSlateBrush* STaskTreeItem::GetBorderImage() const
{
	return bDraggedOver ? FEditorStyle::GetBrush("Menu.Background") : FEditorStyle::GetBrush("NoBorder");
}

FText STaskTreeItem::GetNameText() const
{
	if (TreeItem.IsValid()) {
		return FText::FromName(TreeItem.Pin()->Object->Name);
	}
	return FText::GetEmpty();
}


STaskTreeItem::~STaskTreeItem()
{
	TreeItem.Pin()->OnRenamedRequestEvent.Remove(EnterEditingModeDelegateHandle);
}

void STaskTreeItem::Construct(const FArguments& InArgs)
{
	
	TreeItem = InArgs._TreeItem;
	bool bIsRoot = true;// !InArgs._TreeItem->Parent.IsValid();
	IsItemExpanded = InArgs._IsItemExpanded;
	ChildSlot
		[
			SNew(SBorder)
			.BorderImage(this, &STaskTreeItem::GetBorderImage)
			.Padding(FMargin(0, bIsRoot ? 6 : 0, 0, 0))	// For root items in the tree, give them a little breathing room on the top
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				[
					SAssignNew(InlineRenameWidget, SInlineEditableTextBlock)
					.Text(this, &STaskTreeItem::GetNameText)
					.ToolTipText(this, &STaskTreeItem::GetNameText)
					.Font(FEditorStyle::GetFontStyle(bIsRoot ? "ContentBrowser.SourceTreeRootItemFont" : "ContentBrowser.SourceTreeItemFont"))
					.HighlightText(FText::FromString("HeightLightText"))
					.OnTextCommitted(this, &STaskTreeItem::HandleNameCommitted)
					.OnVerifyTextChanged(this, &STaskTreeItem::VerifyNameChanged)
					.IsSelected_Lambda([]() {return false; })
					.IsReadOnly(false)
		
				]
			]
		];

	if (InlineRenameWidget.IsValid()) {
		EnterEditingModeDelegateHandle = TreeItem.Pin()->OnRenamedRequestEvent.AddSP(InlineRenameWidget.Get(), &SInlineEditableTextBlock::EnterEditingMode);
	}
}


void STaskTreeItem::HandleNameCommitted(const FText& NewText, ETextCommit::Type CommitInfo)
{
	if (TreeItem.IsValid())
	{
		TSharedPtr<FTaskTreeItem> TreeItemPtr = TreeItem.Pin();
		TreeItemPtr->Object->Name = FName(*NewText.ToString());
	}
}


bool STaskTreeItem::ValidateDragDrop(const FGeometry& MyGeometry,
	const FDragDropEvent& DragDropEvent, bool& OutIsKnownDragOperation) const
{
	OutIsKnownDragOperation = false;

	TSharedPtr<FTaskTreeItem> TreeItemPinned = TreeItem.Pin();
	return TreeItemPinned.IsValid();
}


void STaskTreeItem::OnDragEnter(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{

}
void STaskTreeItem::OnDragLeave(const FDragDropEvent& DragDropEvent)
{

}

FReply STaskTreeItem::OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	return FReply::Handled();
}

FReply STaskTreeItem::OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	return FReply::Handled();
}

void STaskTreeItem::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{

}
