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

class STaskNodeDefault : public STaskNodeBase
{
public:
	SLATE_BEGIN_ARGS(STaskNodeDefault) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphNode* InNode);


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
				+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(FMargin(0,15,5,0))
				[
					SNew(SMultiLineEditableText).WrapTextAt(90).WrappingPolicy(ETextWrappingPolicy::AllowPerCharacterWrapping)
					 .AllowMultiLine(true).AutoWrapText(true)
					.Text(FText::FromString(TEXT("[杀怪任务]杀死20只在田里捣乱的鸡")))
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
