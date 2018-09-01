#pragma once

#include "SColorCmpGroup.h"
#include "Widgets/SBoxPanel.h"
#include "SScrollBox.h"

#define LOCTEXT_NAMESPACE "HSVColorPlugin"

class SColorAnalysisPanel
	: public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SColorAnalysisPanel)
	{}

	SLATE_END_ARGS()

	BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
	void Construct(const FArguments& Args)
	{
		Panel = SNew(SVerticalBox);
		TestColor = FLinearColor(1, 1, 1, 1);

		ChildSlot[ SNew(SScrollBox)
			+ SScrollBox::Slot().Padding(30, 5, 30, 5)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot().AutoHeight()
				[
					SAssignNew( Panel, SVerticalBox)
				]
			]
		];
	}
	END_SLATE_FUNCTION_BUILD_OPTIMIZATION

	FReply AddGroup(TSharedRef<SColorCmpGroup> Group)
	{
		Groups.Add(Group);
		Panel->AddSlot().AutoHeight()[Group ];
		return FReply::Handled();
	}

	FReply CleanGroup()
	{
		List.Empty();
		for (auto Group : Groups) {
			Group->ClearRows();
		}
		Groups.Empty();
		Panel->ClearChildren();
		return FReply::Handled();
	}

	void ApplyNewColor() {
		for (auto Group : Groups) {
			Group->ApplyNewColor();
		}
	}

	void ApplyOldColor() {
		for (auto Group : Groups) {
			Group->ApplyOldColor();
		}
	}

	void ApplyHSVAdjusting(FLinearColor hsvOffset) {
		for (auto Group : Groups) {
			Group->ApplyHSVAdjusting(hsvOffset);
		}
	}

	void AddParicleSystem(UObject* PS) {
		PS->Modify();
		List.Add(PS);
	}

private:
	FText                              Path;
	TAttribute<FLinearColor>           TestColor;
	TSharedPtr<SVerticalBox>           Panel;
	TArray<TSharedRef<SColorCmpGroup>> Groups;
	TArray<UObject*> List;
};


#undef LOCTEXT_NAMESPACE