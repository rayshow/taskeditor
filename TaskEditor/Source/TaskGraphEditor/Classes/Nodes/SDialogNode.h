// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "KismetNodes/SGraphNodeK2Base.h"
#include "STaskNodeBase.h"
#include "SButton.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "Widgets/Text/SMultiLineEditableText.h"
#include "Widgets/SBoxPanel.h"
#include "Expression/TaskSystemExpressionDialog.h"

class SDialogNode : public STaskNodeBase
{
private:
	UTaskSystemExpression1In2Dialog * DialogExpr;

public:
	SLATE_BEGIN_ARGS(SDialogNode) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphNode* InNode)
	{
		auto TaskNode = Cast<UTaskSystemGraphNode>(InNode);
		check(TaskNode);
		DialogExpr = Cast<UTaskSystemExpression1In2Dialog>(TaskNode->Expression);
		check(DialogExpr);

		this->GraphNode = InNode;
		this->SetCursor(EMouseCursor::CardinalCross);
		this->UpdateGraphNode();
	}


	// Override this to add widgets below the pins but above advanced view arrow
	virtual void CreateBelowPinControls(TSharedPtr<SVerticalBox> MainBox) override {

		MainBox->AddSlot()
			.Padding(10)
			.AutoHeight()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot().HAlign(HAlign_Left).VAlign(VAlign_Center)
				.AutoWidth()
				[
					SNew(SMultiLineEditableText)
					.WrappingPolicy(ETextWrappingPolicy::AllowPerCharacterWrapping)
					.AllowMultiLine(true).AutoWrapText(true).WrapTextAt(120)
					.Text_Lambda([this]() {
					  return DialogExpr->DialogContent.IsEmpty() ? FText::FromString(TEXT("请输入对话内容。"))
						: DialogExpr->DialogContent;
					})
				]
			];
	}

};
