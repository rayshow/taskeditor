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

class SSpawnMonsterNode : public STaskNodeBase
{
public:
	SLATE_BEGIN_ARGS(SSpawnMonsterNode) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphNode* InNode)
	{
		this->GraphNode = InNode;
		this->SetCursor(EMouseCursor::CardinalCross);
		this->UpdateGraphNode();
	}

	// Override this to add widgets below the pins but above advanced view arrow
	virtual void CreateBelowPinControls(TSharedPtr<SVerticalBox> MainBox) override {

		//if (GraphNode && MainBox.IsValid())
		//{
		//	int32 LeftPinCount = InputPins.Num();
		//	int32 RightPinCount = OutputPins.Num();

		//	const float NegativeHPad = 10;
		//	const float ExtraPad = 0.0f;

		//	auto Box = SNew(SHorizontalBox) 
		//		+ SHorizontalBox::Slot().FillWidth(40)
		//		.VAlign(VAlign_Center)
		//		.HAlign(HAlign_Center).Padding(FMargin(0, 15, 5, 0))
		//	[
		//		SNew(STextBlock).Text(FText::FromString("fdasfdsa")).ColorAndOpacity(FLinearColor(1, 0, 0, 1))
		//	];


		//	// Place preview widget based on where the least pins are
		//	if ((LeftPinCount < RightPinCount) || (RightPinCount == 0))
		//	{
		//		LeftNodeBox->AddSlot()
		//			.Padding(FMargin(NegativeHPad + ExtraPad, 0.0f, 0.0f, 0.0f))
		//			.AutoHeight()
		//			[
		//				Box
		//			];
		//	}
		//	else if (LeftPinCount > RightPinCount)
		//	{
		//		RightNodeBox->AddSlot()
		//			.Padding(FMargin(NegativeHPad + ExtraPad, 0.0f, 0.0f, 0.0f))
		//			.AutoHeight()
		//			[
		//				Box
		//			];
		//	}
		//	else
		//	{
		//		MainBox->AddSlot()
		//			.Padding(10)
		//			.AutoHeight()
		//			[
		//				SNew(SHorizontalBox)
		//				+ SHorizontalBox::Slot()
		//				.AutoWidth()
		//				[
		//					Box
		//				]
		//			];
		//	}
		//}
	}
};
