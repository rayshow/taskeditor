#pragma once

#include "SColorCmpRow.h"
#include "Widgets/SBoxPanel.h"

#define LOCTEXT_NAMESPACE "HSVColorPlugin"

class SColorCmpGroup
	: public SVerticalBox
{
public:

	BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
	void Construct(const FArguments& Args)
	{
		AddPathLabel();
		SVerticalBox::Construct(Args);
	}
	END_SLATE_FUNCTION_BUILD_OPTIMIZATION

	FReply AddNewRow(TSharedRef<SColorCmpRow> row) {
		Rows.Add(row);
		AddSlot().Padding(0,5,0,5).AutoHeight() [ row ];
		return FReply::Handled();
	}

	FReply ClearRows() {
		Rows.Empty();
		ClearChildren();
		AddPathLabel();
		return FReply::Handled();
	}

	void AddPathLabel() {
		Label = SNew(STextBlock);//.Text(FText::Format(LOCTEXT("M1", "Path: {0}"), Path));
		AddSlot()[Label.ToSharedRef()];
	}

	void SetPath(const FText& InPath) {
		Path = InPath;
		Label->SetText(FText::Format(LOCTEXT("M1", "Path: {0}"), Path));
	}

	FReply ApplyNewColor() {
		for (auto Row : Rows) {
			Row->ApplyNewColor();
		}
		return FReply::Handled();
	}

	FReply ApplyOldColor() {
		for (auto Row : Rows) {
			Row->ApplyOldColor();
		}
		return FReply::Handled();
	}

	FReply ApplyHSVAdjusting(FLinearColor hsvOffset) {
		for (auto Row : Rows) {
			Row->ApplyHSVAdjusting(hsvOffset);
		}
		return FReply::Handled();
	}

private:
	FText                             Path;
	TArray< TSharedRef<SColorCmpRow>> Rows;
	TSharedPtr<STextBlock> Label;

};

#undef LOCTEXT_NAMESPACE