// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Pins/SEventPin.h"
#include "Pins/SSpawnMonsterPin.h"
#include "Pins/SWeatherPin.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Input/STextComboBox.h"
#include "Expression/TaskSystemExpressionWeather.h"


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


void SSpawnMonsterPin::Construct(const FArguments& InArgs, UEdGraphPin* InPin)
{
	SEventPin::Construct(SEventPin::FArguments(), InPin);
	Number = 0;
}

TSharedRef<SWidget>	SSpawnMonsterPin::GetDefaultValueWidget()
{
	return SNew(SHorizontalBox)
		+ SHorizontalBox::Slot().AutoWidth()
		[
			SNew(STextBlock).Text(FText::FromString(TEXT(" 怪物ID: ")))
		]
		+ SHorizontalBox::Slot().AutoWidth()
		[
			SNew(SEditableTextBox).MinDesiredWidth(50).Text(FText::FromString("113211"))
		] 
		+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
		[
			SNew(STextBlock).Text(FText::FromString(TEXT(" 数量: ")))
		]
		+ SHorizontalBox::Slot().AutoWidth()
		[
			SNew(SNumericEntryBox<int>)
			.MinDesiredValueWidth(30)
			.AllowSpin(true)
			.MinSliderValue(1)
			.MaxSliderValue(10000)
			.Value_Lambda([&]() { return Number; })
			.OnValueCommitted_Lambda([&](int N, ETextCommit::Type) {   Number = N; })
			.OnValueChanged_Lambda([&](int N) { Number = N;  })
		];
}

TSharedRef<SWidget> SSpawnMonsterPin::GetLabelWidget(const FName& InPinLabelStyle)
{
	return 
		SNew(SHorizontalBox) 
		+ SHorizontalBox::Slot().AutoWidth().HAlign(HAlign_Center)
		[
			SNew(STextBlock)
			.Text(this, &SSpawnMonsterPin::GetPinLabel)
			.Visibility(this, &SSpawnMonsterPin::GetPinLabelVisibility)
			.ColorAndOpacity(this, &SSpawnMonsterPin::GetPinTextColor)
		] ;
}


void SWeatherPin::Construct(const FArguments& InArgs, UEdGraphPin* InPin)
{
	SEventPin::Construct(SEventPin::FArguments(), InPin);
	During = 0;
	
}

TArray<TSharedPtr<FString>>* SWeatherPin::ConstructWeatherOptions()
{
	static bool Initialize = false;
	static TArray<TSharedPtr<FString>> Options;
	if (!Initialize)
	{
		const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("TaskSystemWeatherType"), true);
		check(EnumPtr);

		for (int i = 0; i < TWT_MAX; ++i)
		{
			auto index = EnumPtr->GetIndexByValue(i);
			Options.Add( MakeShareable( new FString(EnumPtr->GetEnumText(index).ToString())));
		}
		Initialize = true;
	}
	return &Options;
}

TSharedPtr<FString> SWeatherPin::GetStringByEnum(int Type)
{
	auto& Options = *SWeatherPin::ConstructWeatherOptions();
	return Options[Type];
}

TSharedRef<SWidget>	SWeatherPin::GetDefaultValueWidget()
{
	return SNew(SHorizontalBox)
		+ SHorizontalBox::Slot().AutoWidth()
		[
			SNew(STextComboBox).OptionsSource(ConstructWeatherOptions())
			.InitiallySelectedItem(GetStringByEnum(1))
		]
	+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
		[
			SNew(STextBlock).Text(FText::FromString(TEXT(" 时长: ")))
		]
	+ SHorizontalBox::Slot().AutoWidth()
		[
			SNew(SNumericEntryBox<int>)
			.MinDesiredValueWidth(30)
			.AllowSpin(true)
			.MinSliderValue(1)
			.MaxSliderValue(10000)
			.Value_Lambda([&]() { return During; })
			.OnValueCommitted_Lambda([&](int N, ETextCommit::Type) {   During = N; })
			.OnValueChanged_Lambda([&](int N) { During = N;  })
		];
}
	