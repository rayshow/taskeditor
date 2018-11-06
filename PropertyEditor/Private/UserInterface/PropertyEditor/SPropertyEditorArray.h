// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Misc/Attribute.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Fonts/SlateFontInfo.h"
#include "EditorStyleSet.h"
#include "Presentation/PropertyEditor/PropertyEditor.h"
#include "PropertyEditorHelpers.h"
#include "UserInterface/PropertyEditor/PropertyEditorConstants.h"
#include "Widgets/Text/STextBlock.h"
#include "ApplyInterface.h"

#define LOCTEXT_NAMESPACE "PropertyEditor"

class SPropertyEditorArray : public SCompoundWidget, public ApplyInterface
{
public:

	SLATE_BEGIN_ARGS( SPropertyEditorArray )
		: _Font( FEditorStyle::GetFontStyle( PropertyEditorConstants::PropertyFontStyle ) ) 
		{}
		SLATE_ATTRIBUTE( FSlateFontInfo, Font )
	SLATE_END_ARGS()

	void Construct( const FArguments& InArgs, const TSharedRef<FPropertyEditor>& InPropertyEditor )
	{
		PropertyEditor = InPropertyEditor;

		TAttribute<FText> TextAttr;
		if( PropertyEditorHelpers::IsStaticArray( *InPropertyEditor->GetPropertyNode() ) )
		{
			// Static arrays need special case handling for their values
			TextAttr.Set( GetArrayTextValue() );
		}
		else
		{
			TextAttr.Bind( this, &SPropertyEditorArray::GetArrayTextValue );
		}

		ChildSlot
		.Padding( 0.0f, 0.0f, 2.0f, 0.0f )
		[
			SNew( STextBlock )
			.Text( TextAttr )
			.Font( InArgs._Font )
		];

		SetEnabled( TAttribute<bool>( this, &SPropertyEditorArray::CanEdit ) );
	}

	static bool Supports( const TSharedRef<FPropertyEditor>& InPropertyEditor )
	{
		const UProperty* NodeProperty = InPropertyEditor->GetProperty();

		return PropertyEditorHelpers::IsStaticArray( *InPropertyEditor->GetPropertyNode() ) 
			|| PropertyEditorHelpers::IsDynamicArray( *InPropertyEditor->GetPropertyNode() );
	}

	void GetDesiredWidth( float& OutMinDesiredWidth, float &OutMaxDesiredWidth )
	{
		OutMinDesiredWidth = 170.0f;
		OutMaxDesiredWidth = 170.0f;
	}

	virtual void Apply() override
	{
		const TSharedRef< IPropertyHandle > PropertyHandle = PropertyEditor->GetPropertyHandle();
		auto ArrayProp = PropertyHandle->AsArray();
		auto MapProp = PropertyHandle->AsMap();
		auto SetProp = PropertyHandle->AsSet();
		int  FromNum = PropertyEditor->GetPropertyNode()->GetNumChildNodes();

		FReadAddressList ReadAddresses;
		PropertyEditor->GetPropertyNode()->GetReadAddress(false, ReadAddresses, false, false, false, true);

		auto NodeProperty = PropertyEditor->GetPropertyNode()->GetProperty();
		UArrayProperty* Array = Cast<UArrayProperty>(NodeProperty);
		USetProperty* Set = Cast<USetProperty>(NodeProperty);
		UMapProperty* Map = Cast<UMapProperty>(NodeProperty);
		int Index = -1;
		bool bAddedMapEntry = false;

		for (int32 i = 0; i < ReadAddresses.Num(); ++i)
		{
			void* Addr = ReadAddresses.GetAddress(i);

			if (Array)
			{
				FScriptArrayHelper	ArrayHelper(Array, Addr);
				int ToNum = ArrayHelper.Num();
				if (FromNum > ToNum)
				{
					ArrayHelper.AddValues(FromNum - ToNum);
				}
				else if(FromNum < ToNum) {
					ArrayHelper.RemoveValues(0, ToNum - FromNum);
				}
			}
			//else if (Set)
			//{
			//	FScriptSetHelper	SetHelper(Set, Addr);
			//	
			//	int ToNum = SetHelper.Num();
			//	if (FromNum > ToNum)
			//	{
			//		for (int i = 0; i < FromNum-ToNum; ++i)
			//		{
			//			Index = SetHelper.AddDefaultValue_Invalid_NeedsRehash();
			//		}
			//	}
			//	else {
			//		SetHelper.RemoveAt(0, FromNum - ToNum);
			//	}

			//	SetHelper.Rehash();
			//}
			//else if (Map)
			//{
			//	FScriptMapHelper	MapHelper(Map, Addr);
			//	Index = MapHelper.AddDefaultValue_Invalid_NeedsRehash();
			//	MapHelper.Rehash();

			//	uint8* PairPtr = MapHelper.GetPairPtr(Index);
			//	bAddedMapEntry = true;
			//}

		}
		
	}

private:
	FText GetArrayTextValue() const
	{
		return FText::Format( LOCTEXT("NumArrayItemsFmt", "{0} Array elements"), FText::AsNumber(PropertyEditor->GetPropertyNode()->GetNumChildNodes()) );
	}

	/** @return True if the property can be edited */
	bool CanEdit() const
	{
		return PropertyEditor.IsValid() ? !PropertyEditor->IsEditConst() : true;
	}
private:
	TSharedPtr<FPropertyEditor> PropertyEditor;
};

#undef LOCTEXT_NAMESPACE
