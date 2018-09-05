// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.


#include "Pins/SEventPin.h"
#include "Widgets/Layout/SSpacer.h"

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

TSharedRef<SWidget>	SEventPin::GetDefaultValueWidget()
{
	return SNew(STextBlock); // not used for exec pin		
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
