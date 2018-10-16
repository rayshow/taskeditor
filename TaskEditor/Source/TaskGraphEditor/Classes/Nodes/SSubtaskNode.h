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
#include "Expression/TaskSystemExpressionSubtask.h"
#include "TaskSystemGraphNode.h"

class SSubtaskNode : public STaskNodeBase
{
private:
	UTaskSystemExpressionSubtask * Expr;

public:
	SLATE_BEGIN_ARGS(SSubtaskNode) {}
	SLATE_END_ARGS()


	void Construct(const FArguments& InArgs, UEdGraphNode* InNode)
	{
		auto TaskNode = Cast<UTaskSystemGraphNode>(InNode);
		check(TaskNode);
		Expr = Cast<UTaskSystemExpressionSubtask>( TaskNode->Expression);
		check(Expr);

		if (Expr->OnExpressionChanged.IsBound())
			Expr->OnExpressionChanged.Unbind();
		Expr->OnExpressionChanged.BindRaw(this, &SSubtaskNode::OnExpressionChanged);

		this->GraphNode = InNode;
		this->SetCursor(EMouseCursor::CardinalCross);
		this->UpdateGraphNode();
	}

	void OnExpressionChanged(FString PropertyName)
	{
		if (auto TaskNode = Cast<UTaskSystemGraphNode>(GraphNode)) {
			TaskNode->RecreateAndLinkNode();
		}

		this->UpdateGraphNode();
	}


	// Override this to add widgets below the node and pins
	virtual void CreateBelowWidgetControls(TSharedPtr<SVerticalBox> MainBox) override {

	}

	// Override this to add widgets below the pins but above advanced view arrow
	virtual void CreateBelowPinControls(TSharedPtr<SVerticalBox> MainBox) override {

		if (GraphNode && MainBox.IsValid())
		{
			int32 LeftPinCount = InputPins.Num();
			int32 RightPinCount = OutputPins.Num();

			const float NegativeHPad = 10;
			const float ExtraPad = 0.0f;

			auto Box = SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Center)
				[
					SNew(SMultiLineEditableText)
					.WrapTextAt(110).WrappingPolicy(ETextWrappingPolicy::AllowPerCharacterWrapping)
				.AllowMultiLine(true).AutoWrapText(true)
				.Text_Lambda([this]() {

				return FText::Format(NSLOCTEXT("SubTargetNode", "NodeContent", "待添加显示文本"),
					Expr->TaskDesc.IsEmpty() ?
					FText::FromString(TEXT("请输入任务描述。")) : Expr->TaskDesc
				);
			})
				];

			MainBox->AddSlot()
				.Padding(10)
				.AutoHeight()
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							Box
						]
					];
		}
	}

	TSharedRef<SWidget> CreateNodeContentArea()
	{
		// NODE CONTENT AREA
		return SNew(SBorder)
			.BorderImage(FEditorStyle::GetBrush("NoBorder"))
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.Padding(FMargin(0, 3))
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Left)
				.FillWidth(1.0f)
				[
					// LEFT
					SAssignNew(LeftNodeBox, SVerticalBox)
				]
			+ SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Right)
				[
					SAssignNew(RightNodeBox, SVerticalBox)
				]
			];
	}

};
