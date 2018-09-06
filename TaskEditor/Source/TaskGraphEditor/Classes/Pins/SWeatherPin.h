// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SWidget.h"
#include "SEventPin.h"

class SWeatherPin : public SEventPin
{
public:
	SLATE_BEGIN_ARGS(SSpawnMonsterPin) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin);
private:

	//~ Begin SGraphPin Interface
	virtual TSharedRef<SWidget>	GetDefaultValueWidget() override;
	//~ End SGraphPin Interface

	int During;

	static TArray<TSharedPtr<FString>>* ConstructWeatherOptions();
	static TSharedPtr<FString> GetStringByEnum(int Type);
};
