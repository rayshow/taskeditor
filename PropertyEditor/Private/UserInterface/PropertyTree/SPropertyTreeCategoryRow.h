// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Layout/Margin.h"
#include "Widgets/Views/STableViewBase.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Views/STableRow.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/SBoxPanel.h"
#include "EditorStyleSet.h"
#include "UserInterface/PropertyEditor/PropertyEditorConstants.h"
#include "PropertyNode.h"


class FPropertyNode;

class SPropertyTreeCategoryRow : public STableRow< TSharedPtr< class FPropertyNode* > >
{
public:

	SLATE_BEGIN_ARGS( SPropertyTreeCategoryRow )
		: _DisplayName()
	{}
		SLATE_ARGUMENT( FText, DisplayName )
		SLATE_ARGUMENT(TWeakPtr<FPropertyNode>, PropertyNode)
	SLATE_END_ARGS()


	void Construct( const FArguments& InArgs,
		const TSharedRef<STableViewBase>& InOwnerTable)
	{
		auto PropertyNode = InArgs._PropertyNode;
		STableRow< TSharedPtr< class FPropertyNode* > >::Construct(
			STableRow< TSharedPtr< class FPropertyNode* > >::FArguments()
				.Padding(3)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot().AutoWidth()
					[
						SNew(SCheckBox).IsChecked_Lambda([PropertyNode]() {
							if (PropertyNode.IsValid())
							{
								return PropertyNode.Pin()->GetCheckState();
							}
							return ECheckBoxState::Unchecked;
						}).OnCheckStateChanged_Lambda([PropertyNode](ECheckBoxState State) {
							if (PropertyNode.IsValid())
							{
								return PropertyNode.Pin()->SetCheckState(State);
							}
						})
					]
				+ SHorizontalBox::Slot()
					[
						SNew(SBorder)
						.VAlign(VAlign_Center)
						.BorderImage(FEditorStyle::GetBrush("PropertyWindow.CategoryBackground"))
						.Padding(FMargin(5,1,5,0))
						.ForegroundColor(FEditorStyle::GetColor("PropertyWindow.CategoryForeground"))
						[
						
								SNew( STextBlock )
								.Text( InArgs._DisplayName )
								.Font( FEditorStyle::GetFontStyle( PropertyEditorConstants::CategoryFontStyle ) )
			
						]
					]
				]
			, InOwnerTable );
	}
};
