#pragma once

#include"SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "SlateOptMacros.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Colors/SColorBlock.h"
#include "Widgets/Colors/SColorPicker.h"
#include "Framework/Application/SlateApplication.h"

DECLARE_DELEGATE_OneParam(FOnApplyColor, FLinearColor)

#define LOCTEXT_NAMESPACE "ColorPlugin"


class SColorCmpRow
	: public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SColorCmpRow):
		_OldColor { FLinearColor::White},
		_KeyColorOwner{nullptr},
		_IsBegin{true}
	{}

	//attribute to setup
	SLATE_ATTRIBUTE(FLinearColor, OldColor)
	SLATE_ARGUMENT(UDistributionVector*, KeyColorOwner)
	SLATE_ARGUMENT(int, KeyIndex)
	SLATE_ARGUMENT(bool, IsBegin)

	//delegate
	SLATE_EVENT(FOnApplyColor, OnApplyOldColor)
	SLATE_EVENT(FOnApplyColor, OnApplyNewColor)

	SLATE_END_ARGS()

	BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
	void Construct(const FArguments& Args)
	{
		OldColor = Args._OldColor;
		NewColor = Args._OldColor;
		OnApplyOldColor = Args._OnApplyOldColor;
		OnApplyNewColor = Args._OnApplyNewColor;
		IsBegin = Args._IsBegin;
		KeyIndex = Args._KeyIndex;
		KeyColorOwner = Args._KeyColorOwner;

		ChildSlot[
			SNew(SHorizontalBox)
				+ SHorizontalBox::Slot().Padding(20, 0, 0, 0)
				[
					// Old Color Tips
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot().FillWidth(3)
				[
					SNew(SHorizontalBox) + SHorizontalBox::Slot().Padding(20, 0, 0, 0).HAlign(HAlign_Left)
					[
						SNew(STextBlock).Text(FText::Format(
							IsBegin?LOCTEXT("M2", "Curve Key{0} Begin Color:"):
							LOCTEXT("M2", "Curve Key{0} End Color:"), KeyIndex))
					]
				]
			// New Color Block
			+ SHorizontalBox::Slot().FillWidth(2)
				[
					// Old Color Select
					SNew(SColorBlock).Color(OldColor)
				]
			// New Color Tips
			+ SHorizontalBox::Slot().FillWidth(3)
				[
					SNew(SHorizontalBox) + SHorizontalBox::Slot().Padding(20, 0, 0, 0).HAlign(HAlign_Left)
					[
						SNew(STextBlock).Text(LOCTEXT("M3", "New Color:"))
					]
				]
			// New Color Select
			+ SHorizontalBox::Slot().FillWidth(2)
				[
					SNew(SColorBlock)
					.Color_Lambda([this]() { return NewColor.Get(); })
					.OnMouseButtonDown(this, &SColorCmpRow::OpenColorPicker)
				]
			]
		];
	}
	END_SLATE_FUNCTION_BUILD_OPTIMIZATION

	//open color picker when new-color box clicked
	FReply OpenColorPicker(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
	{
		TSharedRef<SWindow> NewSlateWindow = FSlateApplication::Get().AddWindow(
			SNew(SWindow)
			.Title(LOCTEXT("ColorPickerTest-WindowTitle-StandardColor", "Standard Color"))
			.ClientSize(SColorPicker::DEFAULT_WINDOW_SIZE)
		);

		TSharedPtr<SColorPicker> ColorPicker = SNew(SColorPicker)
			.TargetColorAttribute_Lambda([&]() { return NewColor.Get(); })
			.OnColorCommitted_Lambda([&](FLinearColor ChoosedColor) {
			NewColor.Set(ChoosedColor);
		})
		.ParentWindow(NewSlateWindow);

		NewSlateWindow->SetContent(ColorPicker.ToSharedRef());
		return FReply::Handled();
	}
	
	void ApplyColor(FLinearColor Color) {
		if (KeyColorOwner)
		{
			if (IsBegin) {
				KeyColorOwner->SetKeyOut(0, KeyIndex, Color.R);
				KeyColorOwner->SetKeyOut(1, KeyIndex, Color.G);
				KeyColorOwner->SetKeyOut(2, KeyIndex, Color.B);
			}
			else {
				KeyColorOwner->SetKeyOut(3, KeyIndex, Color.R);
				KeyColorOwner->SetKeyOut(4, KeyIndex, Color.G);
				KeyColorOwner->SetKeyOut(5, KeyIndex, Color.B);
			}
			KeyColorOwner->SetFlags(RF_Transactional);
			KeyColorOwner->Modify();
			
		}
	}

	//called when recover old color
	FReply ApplyOldColor() {
		if (bIsChanged) {
			ApplyColor(OldColor.Get());
		}
		return FReply::Handled();
	}

	//called when use new color
	FReply ApplyNewColor() {
		ApplyColor(NewColor.Get());
		bIsChanged = true;
		return FReply::Handled();
	}

	void ApplyHSVAdjusting(FLinearColor HSVOffset) {
		FLinearColor Color = OldColor.Get();
		FLinearColor HSV = Color.LinearRGBToHSV();

		FLinearColor RoundHSV;
		float R = HSV.R + HSVOffset.R + 360;
		RoundHSV.R = R - ((int)(R / 360)) * 360 ;
		RoundHSV.G = FMath::Clamp(HSV.G*HSVOffset.G, 0.0f, 1.0f);
		RoundHSV.B = FMath::Max(0.0f, HSV.B*HSVOffset.B);
		RoundHSV.A = 1;

		NewColor.Set(RoundHSV.HSVToLinearRGB());
	}

private:
	TAttribute<FLinearColor> OldColor;
	TAttribute<FLinearColor> NewColor;
	bool                     bIsChanged;
	UDistributionVector*     KeyColorOwner;
	int                      KeyIndex;
	bool                     IsBegin;

	FOnApplyColor   OnApplyOldColor;
	FOnApplyColor   OnApplyNewColor;
};


#undef LOCTEXT_NAMESPACE