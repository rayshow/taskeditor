// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Pins/SSelectNPCPin.h"
#include "Modules/ModuleManager.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SComboButton.h"
#include "Editor.h"
#include "IContentBrowserSingleton.h"
#include "ContentBrowserModule.h"
#include "ScopedTransaction.h"
#include "Engine/Selection.h"
#include "AssetRegistryModule.h"

#define LOCTEXT_NAMESPACE "SSelectNPCPin"

namespace GraphPinObjectDefs
{
	// Active Combo pin alpha
	static const float ActiveComboAlpha = 1.f;
	// InActive Combo pin alpha
	static const float InActiveComboAlpha = 0.6f;
	// Active foreground pin alpha
	static const float ActivePinForegroundAlpha = 1.f;
	// InActive foreground pin alpha
	static const float InactivePinForegroundAlpha = 0.15f;
	// Active background pin alpha
	static const float ActivePinBackgroundAlpha = 0.8f;
	// InActive background pin alpha
	static const float InactivePinBackgroundAlpha = 0.4f;
};

void SSelectNPCPin::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
{
	InGraphPinObj->bNotConnectable = false;
	CachePinIcons();
	SGraphPin::Construct(SGraphPin::FArguments(), InGraphPinObj);
}

void SSelectNPCPin::CachePinIcons()
{
	CachedImg_Pin_ConnectedHovered = FEditorStyle::GetBrush(TEXT("Graph.PosePin.ConnectedHovered"));
	CachedImg_Pin_Connected = FEditorStyle::GetBrush(TEXT("Graph.PosePin.Connected"));
	CachedImg_Pin_DisconnectedHovered = FEditorStyle::GetBrush(TEXT("Graph.PosePin.DisconnectedHovered"));
	CachedImg_Pin_Disconnected = FEditorStyle::GetBrush(TEXT("Graph.PosePin.Disconnected"));
}

const FSlateBrush* SSelectNPCPin::GetPinIcon() const
{
	const FSlateBrush* Brush = NULL;
	if (IsConnected()) {
		Brush = IsHovered() ? CachedImg_Pin_ConnectedHovered : CachedImg_Pin_Connected;
	}
	else {
		Brush = IsHovered() ? CachedImg_Pin_DisconnectedHovered : CachedImg_Pin_Disconnected;
	}
	return Brush;
}


TSharedRef<SWidget>	SSelectNPCPin::GetDefaultValueWidget()
{
	if (AllowSelfPinWidget())
	{
		if (GraphPinObj->GetSchema()->IsSelfPin(*GraphPinObj))
		{
			return SNew(SEditableTextBox)
				.Style(FEditorStyle::Get(), "Graph.EditableTextBox")
				.Text(this, &SSelectNPCPin::GetValue)
				.SelectAllTextWhenFocused(false)
				.Visibility(this, &SSelectNPCPin::GetDefaultValueVisibility)
				.IsReadOnly(true)
				.ForegroundColor(FSlateColor::UseForeground());
		}
	}
	// Don't show literal buttons for component type objects
	if (GraphPinObj->GetSchema()->ShouldShowAssetPickerForPin(GraphPinObj))
	{
		return
			SNew(SHorizontalBox)
			.Visibility(this, &SGraphPin::GetDefaultValueVisibility)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(2, 0)
			.MaxWidth(50.0f)
			[
				SAssignNew(AssetPickerAnchor, SComboButton)
				.ButtonStyle(FEditorStyle::Get(), "PropertyEditor.AssetComboStyle")
				.ForegroundColor(this, &SSelectNPCPin::OnGetComboForeground)
				.ContentPadding(FMargin(2, 2, 2, 1))
				.ButtonColorAndOpacity(this, &SSelectNPCPin::OnGetWidgetBackground)
				.MenuPlacement(MenuPlacement_BelowAnchor)
				.ButtonContent()
				[
					SNew(STextBlock)
					.ColorAndOpacity(this, &SSelectNPCPin::OnGetComboForeground)
					.TextStyle(FEditorStyle::Get(), "PropertyEditor.AssetClass")
					.Font(FEditorStyle::GetFontStyle("PropertyWindow.NormalFont"))
					.Text(this, &SSelectNPCPin::OnGetComboTextValue)
					.ToolTipText(this, &SSelectNPCPin::GetObjectToolTip)
				]
				.OnGetMenuContent(this, &SSelectNPCPin::GenerateAssetPicker)
			];
	}

	return SNullWidget::NullWidget;
}

FOnClicked SSelectNPCPin::GetOnUseButtonDelegate()
{
	return FOnClicked::CreateSP(this, &SSelectNPCPin::OnClickUse);
}

FOnClicked SSelectNPCPin::GetOnBrowseButtonDelegate()
{
	return FOnClicked::CreateSP(this, &SSelectNPCPin::OnClickBrowse);
}

FText SSelectNPCPin::GetObjectToolTip() const
{
	return GetValue();
}

FString SSelectNPCPin::GetObjectToolTipAsString() const
{
	return GetValue().ToString();
}

FReply SSelectNPCPin::OnClickUse()
{
	FEditorDelegates::LoadSelectedAssetsIfNeeded.Broadcast();

	UClass* ObjectClass = Cast<UClass>(GraphPinObj->PinType.PinSubCategoryObject.Get());
	if (ObjectClass != NULL)
	{
		UObject* SelectedObject = GEditor->GetSelectedObjects()->GetTop(ObjectClass);
		if (SelectedObject != NULL)
		{
			const FScopedTransaction Transaction(NSLOCTEXT("GraphEditor", "ChangeObjectPinValue", "Change Object Pin Value"));
			GraphPinObj->Modify();

			GraphPinObj->GetSchema()->TrySetDefaultObject(*GraphPinObj, SelectedObject);
		}
	}

	return FReply::Handled();
}

FReply SSelectNPCPin::OnClickBrowse()
{
	const FAssetData& AssetData = GetAssetData(false);
	if (AssetData.IsValid())
	{
		TArray<FAssetData> Objects;
		Objects.Add(AssetData);

		GEditor->SyncBrowserToObjects(Objects);
	}
	return FReply::Handled();
}

TSharedRef<SWidget> SSelectNPCPin::GenerateAssetPicker()
{
	// This class and its children are the classes that we can show objects for
	UClass* AllowedClass = Cast<UClass>(GraphPinObj->PinType.PinSubCategoryObject.Get());

	if (AllowedClass == NULL)
	{
		AllowedClass = UObject::StaticClass();
	}

	FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));

	FAssetPickerConfig AssetPickerConfig;
	AssetPickerConfig.Filter.ClassNames.Add(AllowedClass->GetFName());
	AssetPickerConfig.bAllowNullSelection = true;
	AssetPickerConfig.Filter.bRecursiveClasses = true;
	AssetPickerConfig.OnAssetSelected = FOnAssetSelected::CreateSP(this, &SSelectNPCPin::OnAssetSelectedFromPicker);
	AssetPickerConfig.OnAssetEnterPressed = FOnAssetEnterPressed::CreateSP(this, &SSelectNPCPin::OnAssetEnterPressedInPicker);
	AssetPickerConfig.InitialAssetViewType = EAssetViewType::List;
	AssetPickerConfig.bAllowDragging = false;

	// Check with the node to see if there is any "AllowClasses" metadata for the pin
	FString ClassFilterString = GraphPinObj->GetOwningNode()->GetPinMetaData(GraphPinObj->PinName, FName(TEXT("AllowedClasses")));
	if (!ClassFilterString.IsEmpty())
	{
		// Clear out the allowed class names and have the pin's metadata override.
		AssetPickerConfig.Filter.ClassNames.Empty();

		// Parse and add the classes from the metadata
		TArray<FString> CustomClassFilterNames;
		ClassFilterString.ParseIntoArray(CustomClassFilterNames, TEXT(","), true);
		for (auto It = CustomClassFilterNames.CreateConstIterator(); It; ++It)
		{
			AssetPickerConfig.Filter.ClassNames.Add(FName(**It));
		}
	}

	return
		SNew(SBox)
		.HeightOverride(300)
		.WidthOverride(300)
		[
			SNew(SBorder)
			.BorderImage(FEditorStyle::GetBrush("Menu.Background"))
		[
			ContentBrowserModule.Get().CreateAssetPicker(AssetPickerConfig)
		]
		];
}

void SSelectNPCPin::OnAssetSelectedFromPicker(const struct FAssetData& AssetData)
{
	const FAssetData& CurrentAssetData = GetAssetData(true);
	if (CurrentAssetData != AssetData)
	{
		const FScopedTransaction Transaction(NSLOCTEXT("GraphEditor", "ChangeObjectPinValue", "Change Object Pin Value"));
		GraphPinObj->Modify();

		// Close the asset picker
		AssetPickerAnchor->SetIsOpen(false);

		// Set the object found from the asset picker
		GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, AssetData.ObjectPath.ToString());
	}
}

void SSelectNPCPin::OnAssetEnterPressedInPicker(const TArray<FAssetData>& InSelectedAssets)
{
	if (InSelectedAssets.Num() > 0)
	{
		OnAssetSelectedFromPicker(InSelectedAssets[0]);
	}
}

FText SSelectNPCPin::GetValue() const
{
	const FAssetData& CurrentAssetData = GetAssetData(true);
	FText Value;
	if (CurrentAssetData.IsValid())
	{
		Value = FText::FromString(CurrentAssetData.GetFullName());
	}
	else
	{
		if (GraphPinObj->GetSchema()->IsSelfPin(*GraphPinObj))
		{
			Value = FText::FromName(GraphPinObj->PinName);
		}
		else
		{
			Value = FText::GetEmpty();
		}
	}
	return Value;
}

FText SSelectNPCPin::GetObjectName() const
{
	FText Value = FText::GetEmpty();

	if (GraphPinObj != NULL)
	{
		const FAssetData& CurrentAssetData = GetAssetData(true);
		if (CurrentAssetData.IsValid())
		{
			Value = FText::FromString(CurrentAssetData.AssetName.ToString());
			int32 StringLen = Value.ToString().Len();

			//If string is too long, then truncate (eg. "abcdefgijklmnopq" is converted as "abcd...nopq")
			const int32 MaxAllowedLength = 16;
			if (StringLen > MaxAllowedLength)
			{
				//Take first 4 characters
				FString TruncatedStr(Value.ToString().Left(4));
				TruncatedStr += FString(TEXT("..."));

				//Take last 4 characters
				TruncatedStr += Value.ToString().Right(4);
				Value = FText::FromString(TruncatedStr);
			}
		}
	}
	return Value;
}

FText SSelectNPCPin::GetDefaultComboText() const
{
	return LOCTEXT("DefaultComboText", "Select Asset");
}

FText SSelectNPCPin::OnGetComboTextValue() const
{
	FText Value = GetDefaultComboText();

	if (GraphPinObj != nullptr)
	{
		const FAssetData& CurrentAssetData = GetAssetData(true);

		UObject* DefaultObject = GraphPinObj->DefaultObject;
		if (UField* Field = Cast<UField>(DefaultObject))
		{
			Value = Field->GetDisplayNameText();
		}
		else if (CurrentAssetData.IsValid())
		{
			Value = FText::FromString(CurrentAssetData.AssetName.ToString());
		}
	}
	return Value;
}

FSlateColor SSelectNPCPin::OnGetComboForeground() const
{
	float Alpha = (IsHovered() || bOnlyShowDefaultValue) ? GraphPinObjectDefs::ActiveComboAlpha : GraphPinObjectDefs::InActiveComboAlpha;
	return FSlateColor(FLinearColor(1.f, 1.f, 1.f, Alpha));
}

FSlateColor SSelectNPCPin::OnGetWidgetForeground() const
{
	float Alpha = (IsHovered() || bOnlyShowDefaultValue) ? GraphPinObjectDefs::ActivePinForegroundAlpha : GraphPinObjectDefs::InactivePinForegroundAlpha;
	return FSlateColor(FLinearColor(1.f, 1.f, 1.f, Alpha));
}

FSlateColor SSelectNPCPin::OnGetWidgetBackground() const
{
	float Alpha = (IsHovered() || bOnlyShowDefaultValue) ? GraphPinObjectDefs::ActivePinBackgroundAlpha : GraphPinObjectDefs::InactivePinBackgroundAlpha;
	return FSlateColor(FLinearColor(1.f, 1.f, 1.f, Alpha));
}

const FAssetData& SSelectNPCPin::GetAssetData(bool bRuntimePath) const
{
	// For normal assets, the editor and runtime path are the same
	if (GraphPinObj->DefaultObject)
	{
		if (!GraphPinObj->DefaultObject->GetPathName().Equals(CachedAssetData.ObjectPath.ToString(), ESearchCase::CaseSensitive))
		{
			// This always uses the exact object pointed at
			CachedAssetData = FAssetData(GraphPinObj->DefaultObject, true);
		}
	}
	else if (!GraphPinObj->DefaultValue.IsEmpty())
	{
		FName ObjectPath = FName(*GraphPinObj->DefaultValue);
		if (ObjectPath != CachedAssetData.ObjectPath)
		{
			const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

			CachedAssetData = AssetRegistryModule.Get().GetAssetByObjectPath(ObjectPath);

			if (!CachedAssetData.IsValid())
			{
				FString PackageName = FPackageName::ObjectPathToPackageName(GraphPinObj->DefaultValue);
				FString PackagePath = FPackageName::GetLongPackagePath(PackageName);
				FString ObjectName = FPackageName::ObjectPathToObjectName(GraphPinObj->DefaultValue);

				// Fake one
				CachedAssetData = FAssetData(FName(*PackageName), FName(*PackagePath), FName(*ObjectName), UObject::StaticClass()->GetFName());
			}
		}
	}
	else
	{
		if (CachedAssetData.IsValid())
		{
			CachedAssetData = FAssetData();
		}
	}

	return CachedAssetData;
}

#undef LOCTEXT_NAMESPACE
