// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SWidget.h"
#include "SGraphPin.h"
#include "SButton.h"

class SEventPin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SEventPin) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin);
	virtual bool AllowOutputValueWidget() const override { return true; }
	virtual bool AllowConnectedValueWidget() const override { return true; }
protected:
	//~ Begin SGraphPin Interface
	virtual const FSlateBrush* GetPinIcon() const override;
	//~ End SGraphPin Interface

	void CachePinIcons();

	const FSlateBrush* CachedImg_Pin_ConnectedHovered;
	const FSlateBrush* CachedImg_Pin_DisconnectedHovered;
};
