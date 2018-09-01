#pragma once

#include "SGraphPalette.h"

class FTaskEditor;

class STaskSystemPaletteItem : public SGraphPaletteItem
{
public:
	SLATE_BEGIN_ARGS(STaskSystemPaletteItem) {};
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, FCreateWidgetForActionData* const InCreateData);

	TSharedRef<SWidget> CreateHotkeyDisplayWidget(
		const FSlateFontInfo& NameFont, const TSharedPtr<const FInputChord> HotkeyChord);

	virtual FText GetItemTooltip() const override { return ActionPtr.Pin()->GetTooltipDescription(); }
};

class STaskSystemPalette : public SGraphPalette{
public:
	SLATE_BEGIN_ARGS(STaskSystemPalette) {};
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, TWeakPtr<FTaskEditor> InEditorWRef);

	FString GetFilterCategoryName() const { return FString("All"); }
protected:

	virtual TSharedRef<SWidget> OnCreateWidgetForAction(FCreateWidgetForActionData* const InCreateData) override;
	virtual void CollectAllActions(FGraphActionListBuilderBase& OutAllActions) override;
	
	
	void CategorySelectionChanged(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo);

	
	void AddAssetFromAssetRegistry(const FAssetData& InAddedAssetData);
	void RemoveAssetFromRegistry(const FAssetData& InAddedAssetData);
	void RenameAssetFromRegistry(const FAssetData& InAddedAssetData, const FString& InNewName);
	void RefreshAssetInRegistry(const FAssetData& InAddedAssetData);


	/** Pointer back to the material editor that owns us */
	TWeakPtr<FTaskEditor> TaskSystemEditorPtr;

	TArray< TSharedPtr<FString> > CategoryNames;

	TSharedPtr<STextComboBox> CategoryComboBox;
};