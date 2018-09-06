// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SWidget.h"
#include "SEventPin.h"

class SSpawnMonsterPin : public SEventPin
{
public:
	SLATE_BEGIN_ARGS(SSpawnMonsterPin) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin);
private:

	//~ Begin SGraphPin Interface
	virtual TSharedRef<SWidget>	GetDefaultValueWidget() override;
	virtual TSharedRef<SWidget> GetLabelWidget(const FName& InPinLabelStyle) override;
	//~ End SGraphPin Interface

	int Number;
};
