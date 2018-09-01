#include"STaskSystemPalette.h"
#include"Styling/CoreStyle.h"
#include"GraphEditorActions.h"
#include"SpawnNodeCommands.h"
#include"ITaskEditorModule.h"
#include"ModuleManager.h"
#include"AssetRegistryModule.h"
#include"STextComboBox.h"

#define LOCTEXT_NAMESPACE "TaskGraphEditor"


TSharedRef<SWidget> STaskSystemPaletteItem::CreateHotkeyDisplayWidget(
	const FSlateFontInfo& NameFont, const TSharedPtr<const FInputChord> HotkeyChord)
{
	FText HotkeyText;
	if (HotkeyChord.IsValid())
	{
		HotkeyText = HotkeyChord->GetInputText();
	}
	return SNew(STextBlock)
		.Text(HotkeyText)
		.Font(NameFont);
}

void STaskSystemPaletteItem::Construct(const FArguments& InArgs,
	FCreateWidgetForActionData* const InCreateData)
{
	FSlateFontInfo NameFont = FCoreStyle::GetDefaultFontStyle("Regular", 10);

	check(InCreateData->Action.IsValid());

	TSharedPtr<FEdGraphSchemaAction> GraphAction = InCreateData->Action;
	ActionPtr = InCreateData->Action;

	// Get the Hotkey chord if one exists for this action
	TSharedPtr<const FInputChord> HotkeyChord;

	if (FSpawnNewTaskSystemNodeCommands::IsRegistered())
	{
		if (GraphAction->GetTypeId() == FTaskSystemGraphSchemaAction_NewNode::StaticGetTypeId())
		{
			UClass* MaterialExpressionClass = StaticCastSharedPtr<FTaskSystemGraphSchemaAction_NewNode>(GraphAction)->TaskSystemExpressionClass;
			HotkeyChord = FSpawnNewTaskSystemNodeCommands::Get().GetChordByClass(MaterialExpressionClass);
		}
		else if (GraphAction->GetTypeId() == FTaskSystemGraphSchemaAction_NewComment::StaticGetTypeId())
		{
			HotkeyChord = FSpawnNewTaskSystemNodeCommands::Get().GetChordByClass(UTaskSystemExpressionComment::StaticClass());
		}
	}

	// Find icons
	const FSlateBrush* IconBrush = FEditorStyle::GetBrush(TEXT("NoBrush"));
	FSlateColor IconColor = FSlateColor::UseForeground();
	FText IconToolTip = GraphAction->GetTooltipDescription();
	bool bIsReadOnly = false;

	TSharedRef<SWidget> IconWidget = CreateIconWidget(IconToolTip, IconBrush, IconColor);
	TSharedRef<SWidget> NameSlotWidget = CreateTextSlotWidget(NameFont, InCreateData, bIsReadOnly);
	TSharedRef<SWidget> HotkeyDisplayWidget = CreateHotkeyDisplayWidget(NameFont, HotkeyChord);

	// Create the actual widget
	this->ChildSlot
		[
			SNew(SHorizontalBox)
			// Icon slot
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				IconWidget
			]
	// Name slot
	+ SHorizontalBox::Slot()
		.FillWidth(1.f)
		.VAlign(VAlign_Center)
		.Padding(3, 0)
		[
			NameSlotWidget
		]
		// Hotkey slot
		+ SHorizontalBox::Slot()
			.AutoWidth()
			.HAlign(HAlign_Right)
			[
				HotkeyDisplayWidget
			]
		];
}


void STaskSystemPalette::Construct(const FArguments& InArgs, TWeakPtr<FTaskEditor> InTaskEditorPtr)
{
	TaskSystemEditorPtr = InTaskEditorPtr;

	// Create the asset discovery indicator
	ITaskEditorModule& EditorWidgetsModule = FModuleManager::LoadModuleChecked<ITaskEditorModule>("EditorWidgets");
	/*TSharedRef<SWidget> AssetDiscoveryIndicator = 
		EditorWidgetsModule.CreateAssetDiscoveryIndicator(EAssetDiscoveryIndicatorScaleMode::Scale_Vertical);*/

	CategoryNames.Add(MakeShareable(new FString(TEXT("All"))));
	CategoryNames.Add(MakeShareable(new FString(TEXT("Expressions"))));
	CategoryNames.Add(MakeShareable(new FString(TEXT("Functions"))));

	this->ChildSlot
	[
		SNew(SBorder)
		.Padding(2.0f)
		.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(LOCTEXT("Category", "Category: "))
				]
				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				[
					SAssignNew(CategoryComboBox, STextComboBox)
					.OptionsSource(&CategoryNames)
					.OnSelectionChanged(this, &STaskSystemPalette::CategorySelectionChanged)
					.InitiallySelectedItem(CategoryNames[0])
				]
			]

		// Content list
		+ SVerticalBox::Slot()
			[
				SNew(SOverlay)
				+ SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					// Old Expression and Function lists were auto expanded so do the same here for now
					SAssignNew(GraphActionMenu, SGraphActionMenu)
					.OnActionDragged(this, &STaskSystemPalette::OnActionDragged)
					.OnCreateWidgetForAction(this, &STaskSystemPalette::OnCreateWidgetForAction)
					.OnCollectAllActions(this, &STaskSystemPalette::CollectAllActions)
					.AutoExpandActionMenu(true)
				]

				//+ SOverlay::Slot()
				//.HAlign(HAlign_Fill)
				//.VAlign(VAlign_Bottom)
				//.Padding(FMargin(24, 0, 24, 0))
				//	[
				//		// Asset discovery indicator
				//		AssetDiscoveryIndicator
				//	]
			]

		]
	];

	// Register with the Asset Registry to be informed when it is done loading up files.
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::GetModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	AssetRegistryModule.Get().OnAssetAdded().AddSP(this, &STaskSystemPalette::AddAssetFromAssetRegistry);
	AssetRegistryModule.Get().OnAssetRemoved().AddSP(this, &STaskSystemPalette::RemoveAssetFromRegistry);
	AssetRegistryModule.Get().OnAssetRenamed().AddSP(this, &STaskSystemPalette::RenameAssetFromRegistry);
}


TSharedRef<SWidget> STaskSystemPalette::OnCreateWidgetForAction(FCreateWidgetForActionData* const InCreateData)
{
	return SNew(STaskSystemPaletteItem, InCreateData);
}

void STaskSystemPalette::CollectAllActions(FGraphActionListBuilderBase& OutAllActions)
{
	OutAllActions.Empty();
	const UTaskSystemGraphSchema* Schema = GetDefault<UTaskSystemGraphSchema>();
	FGraphActionMenuBuilder ActionMenuBuilder;
	Schema->GetPaletteActions(ActionMenuBuilder, GetFilterCategoryName(), false);
	OutAllActions.Append(ActionMenuBuilder);
}

void STaskSystemPalette::CategorySelectionChanged(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo)
{
	RefreshActionsList(true);
}

void STaskSystemPalette::AddAssetFromAssetRegistry(const FAssetData& InAddedAssetData)
{
	RefreshAssetInRegistry(InAddedAssetData);
}

void STaskSystemPalette::RemoveAssetFromRegistry(const FAssetData& InAddedAssetData)
{
	RefreshAssetInRegistry(InAddedAssetData);
}

void STaskSystemPalette::RenameAssetFromRegistry(const FAssetData& InAddedAssetData, const FString& InNewName)
{
	RefreshAssetInRegistry(InAddedAssetData);
}

void STaskSystemPalette::RefreshAssetInRegistry(const FAssetData& InAddedAssetData)
{
	// Grab the asset class, it will be checked for being a material function.
	//UClass* Asset = FindObject<UClass>(ANY_PACKAGE, *InAddedAssetData.AssetClass.ToString());

	//if (Asset->IsChildOf(UMaterialFunction::StaticClass()))
	{
		RefreshActionsList(true);
	}
}



#undef LOCTEXT_NAMESPACE