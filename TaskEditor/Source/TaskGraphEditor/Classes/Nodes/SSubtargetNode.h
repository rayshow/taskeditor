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

class SSubtargetNode : public STaskNodeBase
{
private:
	UTaskSystemExpressionSubtask * Expr;

public:
	SLATE_BEGIN_ARGS(SSubtargetNode) {}
	SLATE_END_ARGS()


	void Construct(const FArguments& InArgs, UEdGraphNode* InNode)
	{
		auto TaskNode = Cast<UTaskSystemGraphNode>(InNode);
		check(TaskNode);
		Expr = Cast<UTaskSystemExpressionSubtask>( TaskNode->Expression);
		check(Expr);

		this->GraphNode = InNode;
		this->SetCursor(EMouseCursor::CardinalCross);
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
						
						return FText::Format(NSLOCTEXT("SubTargetNode", "NodeContent", " n{1}"),
								Expr->TaskDesc.IsEmpty()?
								FText::FromString(TEXT("请输入任务描述。")) : Expr->TaskDesc
						);
					})
				];


			// Place preview widget based on where the least pins are
			if ((LeftPinCount < RightPinCount) || (RightPinCount == 0))
			{
				LeftNodeBox->AddSlot()
					.Padding(FMargin(NegativeHPad + ExtraPad, 0.0f, 0.0f, 0.0f))
					.AutoHeight()
					[
						Box
					];
			}
			else if (LeftPinCount > RightPinCount)
			{
				RightNodeBox->AddSlot()
					.Padding(FMargin(NegativeHPad + ExtraPad, 0.0f, 0.0f, 0.0f))
					.AutoHeight()
					[
						Box
					];
			}
			else
			{
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
					// RIGHT
					SAssignNew(RightNodeBox, SVerticalBox)
				]
			];
	}

};
