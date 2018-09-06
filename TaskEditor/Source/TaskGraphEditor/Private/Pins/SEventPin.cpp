// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.


#include "Pins/SEventPin.h"
#include "Pins/SEventTextPin.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/SBoxPanel.h"


void SEventPin::Construct(const FArguments& InArgs, UEdGraphPin* InPin)
{
	SGraphPin::Construct(SGraphPin::FArguments(), InPin);

	// Call utility function so inheritors can also call it since arguments can't be passed through
	CachePinIcons();
}

void SEventPin::CachePinIcons()
{
	CachedImg_Pin_Connected = FEditorStyle::GetBrush(TEXT("Graph.ArrayPin.Connected"));
	CachedImg_Pin_Disconnected = FEditorStyle::GetBrush(TEXT("Graph.ArrayPin.Disconnected"));
}



const FSlateBrush* SEventPin::GetPinIcon() const
{
	const FSlateBrush* Brush = NULL;

	if (IsConnected())
	{
		Brush = CachedImg_Pin_Connected;
	}
	else
	{
		Brush = CachedImg_Pin_Disconnected;
	}

	return Brush;
}


void SEventTextPin::Construct(const FArguments& InArgs, UEdGraphPin* InPin)
{
	SEventPin::Construct(InArgs, InPin);
}

TSharedRef<SWidget>	SEventTextPin::GetDefaultValueWidget()
{
	return SNew(SHorizontalBox)
		+ SHorizontalBox::Slot().FillWidth(40)
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Center).Padding(FMargin(0, 15, 5, 0))
		[
			SNew(STextBlock).Text(FText::FromString("fdasfdsa")).ColorAndOpacity(FLinearColor(1, 0, 0, 1))
		];
}