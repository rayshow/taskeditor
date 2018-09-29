// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Input/Reply.h"
#include "Styling/SlateColor.h"
#include "Widgets/SWidget.h"
#include "Widgets/Input/SMenuAnchor.h"
#include "SGraphPin.h"

class SButton;

class SInputNpcIdPin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SInputNpcIdPin) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj);
	virtual bool AllowOutputValueWidget() const override { return true; }
	virtual bool AllowConnectedValueWidget() const override { return true; }

private:
	//~ Begin SGraphPin Interface
	virtual TSharedRef<SWidget>	GetDefaultValueWidget() override;
	virtual const FSlateBrush* GetPinIcon() const override;
	//~ End SGraphPin Interface

	void CachePinIcons();

	const FSlateBrush* CachedImg_Pin_ConnectedHovered;
	const FSlateBrush* CachedImg_Pin_DisconnectedHovered;

	UTaskSystemExpressionSubtask_Common* Expr;

};
