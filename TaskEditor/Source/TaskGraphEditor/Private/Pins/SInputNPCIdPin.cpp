// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Pins/SInputNPCIdPin.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Input/STextComboBox.h"
#include "TaskSystemGraphNode.h"
#include "Expression/TaskSystemExpressionSubtask.h"



void SInputNpcIdPin::Construct(const FArguments& InArgs, UEdGraphPin* InPin)
{
	auto TaskNode = Cast<UTaskSystemGraphNode>(InPin->GetOwningNode());
	check(TaskNode);
	Expr = Cast<UTaskSystemExpressionSubtask>(TaskNode->Expression);
	check(Expr);

	SGraphPin::Construct(SGraphPin::FArguments(), InPin);

	// Call utility function so inheritors can also call it since arguments can't be passed through
	CachePinIcons();
}

void SInputNpcIdPin::CachePinIcons()
{
	CachedImg_Pin_ConnectedHovered = FEditorStyle::GetBrush(TEXT("Graph.PosePin.ConnectedHovered"));
	CachedImg_Pin_Connected = FEditorStyle::GetBrush(TEXT("Graph.PosePin.Connected"));
	CachedImg_Pin_DisconnectedHovered = FEditorStyle::GetBrush(TEXT("Graph.PosePin.DisconnectedHovered"));
	CachedImg_Pin_Disconnected = FEditorStyle::GetBrush(TEXT("Graph.PosePin.Disconnected"));
}



const FSlateBrush* SInputNpcIdPin::GetPinIcon() const
{
	const FSlateBrush* Brush = NULL;
	if (IsConnected()) {
		Brush = IsHovered() ? CachedImg_Pin_ConnectedHovered : CachedImg_Pin_Connected;
	}
	else {
		Brush = IsHovered() ? CachedImg_Pin_DisconnectedHovered : CachedImg_Pin_Disconnected;
	}
	return Brush;
}

TSharedRef<SWidget>	SInputNpcIdPin::GetDefaultValueWidget()
{
	return SNew(SHorizontalBox)
		+ SHorizontalBox::Slot().AutoWidth()
		[
			SNew(STextBlock).Text(FText::FromString(TEXT("ID: ")))
		]
	+ SHorizontalBox::Slot().AutoWidth()
		[
			SNew(SNumericEntryBox<int>)
			.MinDesiredValueWidth(30)
			.AllowSpin(true)
			.MinSliderValue(1)
			.MaxSliderValue(1000000000)
			.Value_Lambda([&]() { return
				GetPinObj()->Direction == EGPD_Input ?
				Expr->StartNpcID : Expr->FinishNpcID;
			})
			.OnValueCommitted_Lambda([&](int N, ETextCommit::Type) {
				if (GetPinObj()->Direction == EGPD_Input)
				{
					Expr->StartNpcID = N;
				}
				else {
					Expr->FinishNpcID = N;
				}
			})
			.OnValueChanged_Lambda([&](int N) {
				if (GetPinObj()->Direction == EGPD_Input)
				{
					Expr->StartNpcID = N;
				}
				else {
					Expr->FinishNpcID = N;
				} 
			})
		];
}