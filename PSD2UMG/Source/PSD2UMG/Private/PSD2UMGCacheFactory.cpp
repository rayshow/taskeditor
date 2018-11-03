
/**

Copyright 2018 swd. All rights reserved.

*/

#include "PSD2UMGCacheFactory.h"
//#include "PSD2UMGPrivatePCH.h"

#define NOMINMAX

#include "PSD2UMGCache.h"

#include "Editor/UnrealEd/Public/Editor.h"

#include "Editor/UnrealEd/Classes/Factories/TextureFactory.h"

#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/Components/PanelWidget.h"
#include "Editor/UMGEditor/Classes/WidgetBlueprintFactory.h"
#include "Editor/UMGEditor/Public/WidgetBlueprint.h"
#include "Runtime/UMG/Public/Components/Image.h"

#include "Runtime/AssetRegistry/Public/AssetRegistryModule.h"

#include "Editor/UnrealEd/Public/ObjectTools.h"

#include "PSDParser.h"

#include <algorithm>

UPSD2UMGCacheFactory::UPSD2UMGCacheFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UPSD2UMGCache::StaticClass();
	bCreateNew = false;
	bText = false;
	bEditorImport = true;
	Formats.Add(TEXT("psdumg;PSD File"));
}
bool UPSD2UMGCacheFactory::DoesSupportClass(UClass* Class)
{
	return false;
	return (Class == UPSD2UMGCache::StaticClass());
}
UClass* UPSD2UMGCacheFactory::ResolveSupportedClass()
{
	return UPSD2UMGCache::StaticClass();
}

static UWidgetBlueprint* GetOrCreateWidget(const FString& packageName, const FString& name, EObjectFlags Flags)
{
	UWidgetBlueprint* userWidget = nullptr;

	// try to read
	userWidget = Cast<UWidgetBlueprint>(StaticLoadObject(UWidgetBlueprint::StaticClass(), NULL, *(packageName + TEXT(".") + name)));
	if (userWidget != nullptr)
	{
		return userWidget;
	}

	UWidgetBlueprintFactory* widgetFactory = NewObject<UWidgetBlueprintFactory>();
	widgetFactory->AddToRoot();

	UPackage* widgetPackage = CreatePackage(NULL, *packageName);

	userWidget = (UWidgetBlueprint*)widgetFactory->FactoryCreateNew(
		UWidgetBlueprint::StaticClass(),
		widgetPackage,
		FName(*name),
		Flags,
		nullptr, nullptr);

	widgetFactory->RemoveFromRoot();

	FAssetRegistryModule::AssetCreated(userWidget);

	return userWidget;
}

static UTexture2D* RenewOrCreateTexture(UTextureFactory* factory, const FString& packageName, const FString& name, EObjectFlags Flags, std::shared_ptr<PSDParser::Layer> layer)
{
	if (layer == nullptr) return nullptr;

	UTexture2D* texture = nullptr;
	bool bNew = false;

	// try to read
	texture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *(packageName + TEXT(".") + name)));

	if (texture == nullptr)
	{
		UPackage* texturePackage = CreatePackage(NULL, *packageName);
		texture = factory->CreateTexture2D(texturePackage, FName(*name), Flags);
		bNew = true;
	}

	
	int32_t textureWidth = layer->GetRect().Right - layer->GetRect().Left;
	int32_t textureHeight = layer->GetRect().Bottom - layer->GetRect().Top;

	texture->Source.Init(textureWidth, textureHeight, 1, 1, ETextureSourceFormat::TSF_BGRA8);
	texture->CompressionSettings = TextureCompressionSettings::TC_EditorIcon;
	texture->MipGenSettings = TMGS_NoMipmaps;

	auto dst = (uint8_t*)texture->Source.LockMip(0);
	auto src = (uint8_t*)layer->GetData();

	for (int32_t y = 0; y < textureHeight; y++)
	{
		for (int32_t x = 0; x < textureWidth; x++)
		{
			dst[(x + textureWidth * y) * 4 + 2] = src[(x + textureWidth * y) * 4 + 0];
			dst[(x + textureWidth * y) * 4 + 1] = src[(x + textureWidth * y) * 4 + 1];
			dst[(x + textureWidth * y) * 4 + 0] = src[(x + textureWidth * y) * 4 + 2];
			dst[(x + textureWidth * y) * 4 + 3] = src[(x + textureWidth * y) * 4 + 3];
		}
	}

	texture->Source.UnlockMip(0);

	texture->UpdateResource();

	if (bNew)
	{
		FAssetRegistryModule::AssetCreated(texture);
	}
	
	texture->MarkPackageDirty();

	return texture;
}

struct LayerCollection
{
	std::u16string	Name;

	PSDParser::Rect	Area;

	std::shared_ptr<PSDParser::Layer>	Layer;

	std::vector < std::shared_ptr<LayerCollection>> Children;
};

static std::shared_ptr<LayerCollection> ParseLayers(PSDParser::Document* document, int32_t& currentIndex)
{
	auto ret = std::make_shared<LayerCollection>();

	for (; currentIndex < document->GetLayerCount();)
	{
		auto layer = document->GetLayer(currentIndex);

		if (layer->IsFolderBegin())
		{
			currentIndex++;
			auto collection = ParseLayers(document, currentIndex);
			collection->Name = layer->GetName();

			ret->Children.push_back(collection);
		}
		else if (layer->IsFolderEnd())
		{
			currentIndex++;
			break;
		}
		else
		{
			auto single = std::make_shared<LayerCollection>();
			single->Layer = layer;
			ret->Children.push_back(single);
			currentIndex++;
		}
	}

	// Calculate size
	PSDParser::Rect area;
	area.Right = -INT_MAX;
	area.Left = INT_MAX;
	area.Top = INT_MAX;
	area.Bottom = -INT_MAX;
	
	for (auto child : ret->Children)
	{
		if (child->Layer != nullptr)
		{
			area.Right = std::max(child->Layer->GetRect().Right, area.Right);
			area.Left = std::min(child->Layer->GetRect().Left, area.Left);
			area.Top = std::min(child->Layer->GetRect().Top, area.Top);
			area.Bottom = std::max(child->Layer->GetRect().Bottom, area.Bottom);
		}
		else
		{
			area.Right = std::max(child->Area.Right, area.Right);
			area.Left = std::min(child->Area.Left, area.Left);
			area.Top = std::min(child->Area.Top, area.Top);
			area.Bottom = std::max(child->Area.Bottom, area.Bottom);
		}
	}

	ret->Area = area;

	return ret;
}

static void CalculatePosition(PSDParser::Rect area, PSDParser::Rect parentArea, FVector2D& pos, FAnchors& anchor)
{
	area.Top -= parentArea.Top;
	area.Bottom -= parentArea.Top;
	area.Left -= parentArea.Left;
	area.Right -= parentArea.Left;

	float parentCanvasSizeX = parentArea.Right - parentArea.Left;
	float parentCanvasSizeY = parentArea.Bottom - parentArea.Top;

	auto centerX = (area.Left + area.Right) / 2.0f / parentCanvasSizeX;
	auto centerY = (area.Bottom + area.Top) / 2.0f / parentCanvasSizeY;

	FAnchors anc;
	if (centerX < 0.25f)
	{
		anc.Minimum.X = 0.0f;
		anc.Maximum.X = 0.0f;
	}
	else if (centerX > 0.75f)
	{
		anc.Minimum.X = 1.0f;
		anc.Maximum.X = 1.0f;
	}
	else
	{
		anc.Minimum.X = 0.5f;
		anc.Maximum.X = 0.5f;
	}

	if (centerY < 0.25f)
	{
		anc.Minimum.Y = 0.0f;
		anc.Maximum.Y = 0.0f;
	}
	else if (centerY > 0.75f)
	{
		anc.Minimum.Y = 1.0f;
		anc.Maximum.Y = 1.0f;
	}
	else
	{
		anc.Minimum.Y = 0.5f;
		anc.Maximum.Y = 0.5f;
	}

	anchor = anc;
	pos = FVector2D(area.Left - parentCanvasSizeX * anc.Minimum.X, area.Top - parentCanvasSizeY * anc.Minimum.Y);
}

static void CreateWidgets(
	UCanvasPanel* rootWidget,
	FVector2D canvasSize,
	UTextureFactory* textureFactory,
	UWidgetBlueprint* userWidget,
	FName InName,
	EObjectFlags Flags,
	FString directoryPath,
	std::shared_ptr<LayerCollection> layers)
{
	// scan layers
	TMap<FString, UButton*> buttons;
	TMap<FString, UProgressBar*> progressBars;

	for (int32_t ind = (int32_t)layers->Children.size() - 1; ind >= 0; ind--)
	{
		auto layer = layers->Children[ind]->Layer;

		// define name
		FString name;
		
		if (layers->Children[ind]->Layer == nullptr)
		{
            if(sizeof(TCHAR) == 2)
            {
                name = (const TCHAR*)layers->Children[ind]->Name.c_str();
            }
            else
            {
                TCHAR temp[260];
                for(int c = 0; c < layers->Children[ind]->Name.size(); c++)
                {
                    temp[c] = layers->Children[ind]->Name[c];
                }
                name = (const TCHAR*)temp;
            }
		}
		else
		{
            if(sizeof(TCHAR) == 2)
            {
                name = (const TCHAR*)layers->Children[ind]->Layer->GetName();
            }
            else
            {
                TCHAR temp[260];
                for(int c = 0; c < 260; c++)
                {
                    temp[c] = layers->Children[ind]->Layer->GetName()[c];
                    if(temp[c] == 0) break;
                }
                name = (const TCHAR*)temp;
            }
		}

		FString widgetName = InName.ToString() + TEXT("_") + ObjectTools::SanitizeObjectName(name);
		if (layer == nullptr)
		{
		}
		else if (layer->ObjectType == PSDParser::LayerObjectType::Button)
		{
			widgetName += TEXT("_Button");

			if (layer->AdditionalObjectType == PSDParser::LayerAdditionalObjectType::Normal)
			{
				widgetName += TEXT("_Normal");
			}
			else if (layer->AdditionalObjectType == PSDParser::LayerAdditionalObjectType::Hovered)
			{
				widgetName += TEXT("_Hovered");
			}
			else if (layer->AdditionalObjectType == PSDParser::LayerAdditionalObjectType::Pressed)
			{
				widgetName += TEXT("_Pressed");
			}
		}
		else if (layer->ObjectType == PSDParser::LayerObjectType::ProgressBar)
		{
			widgetName += TEXT("_ProgressBar");

			if (layer->AdditionalObjectType == PSDParser::LayerAdditionalObjectType::Background)
			{
				widgetName += TEXT("_Background");
			}
			else if (layer->AdditionalObjectType == PSDParser::LayerAdditionalObjectType::Fill)
			{
				widgetName += TEXT("_Fill");
			}
			else if (layer->AdditionalObjectType == PSDParser::LayerAdditionalObjectType::Marquee)
			{
				widgetName += TEXT("_Marquee");
			}
		}

		FString packageName = directoryPath + widgetName;

		auto texture = RenewOrCreateTexture(textureFactory, packageName, widgetName, Flags, layer);
		
		// add the widget
		if (layer == nullptr)
		{
			// find canvas panel
			auto widget = userWidget->WidgetTree->FindWidget<UWidget>(*widgetName);
			auto panel = userWidget->WidgetTree->FindWidget<UCanvasPanel>(*widgetName);

			// if an widget which is not an panel exits, fail to create widgets.
			if (widget != nullptr && panel == nullptr)
			{
				continue;
			}

			// create canvas panel
			if (panel == nullptr)
			{
				panel = userWidget->WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), *widgetName);
				rootWidget->AddChild(panel);
			}


			auto slot = Cast<UCanvasPanelSlot>(panel->Slot);
			if (slot != nullptr)
			{
				slot->SetSize(FVector2D(layers->Children[ind]->Area.Right - layers->Children[ind]->Area.Left, layers->Children[ind]->Area.Bottom - layers->Children[ind]->Area.Top));

				FVector2D pos;
				FAnchors anc;

				CalculatePosition(layers->Children[ind]->Area, layers->Area, pos, anc);

				slot->SetAnchors(anc);
				slot->SetPosition(pos);
				slot->SetSize(FVector2D(layers->Children[ind]->Area.Right - layers->Children[ind]->Area.Left, layers->Children[ind]->Area.Bottom - layers->Children[ind]->Area.Top));
			}

			CreateWidgets(
				panel,
				canvasSize,
				textureFactory,
				userWidget,
				InName,
				Flags,
				directoryPath,
				layers->Children[ind]);
		}
		else if (layer->ObjectType == PSDParser::LayerObjectType::Image)
		{
			if (texture == nullptr) continue;

			auto widget = userWidget->WidgetTree->FindWidget<UWidget>(*name);
			auto image = userWidget->WidgetTree->FindWidget<UImage>(*name);

			// if an widget which is not an image exits, fail to create widgets.
			if (widget != nullptr && image == nullptr)
			{
				continue;
			}

			if (image == nullptr)
			{
				image = userWidget->WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), *name);
				rootWidget->AddChild(image);
			}

			image->SetBrushFromTexture(texture, true);

			auto slot = Cast<UCanvasPanelSlot>(image->Slot);
			if (slot != nullptr)
			{
				FVector2D pos;
				FAnchors anc;

				CalculatePosition(layers->Children[ind]->Layer->GetRect(), layers->Area, pos, anc);

				slot->SetAnchors(anc);
				slot->SetPosition(pos);
				slot->SetSize(FVector2D(layer->GetRect().Right - layer->GetRect().Left, layer->GetRect().Bottom - layer->GetRect().Top));
			}
		}
		else if (layer->ObjectType == PSDParser::LayerObjectType::Button)
		{
			if (texture == nullptr) continue;

			UButton* button = nullptr;

			if (buttons.Find(name) != nullptr)
			{
				button = *buttons.Find(name);
			}
			else
			{
				auto widget = userWidget->WidgetTree->FindWidget<UWidget>(*name);
				button = userWidget->WidgetTree->FindWidget<UButton>(*name);

				// if an widget which is not an button exits, fail to create widgets.
				if (widget != nullptr && button == nullptr)
				{
					continue;
				}

				if (button == nullptr)
				{
					button = userWidget->WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), *name);
					rootWidget->AddChild(button);
				}

				buttons.Add(name, button);

				auto slot = Cast<UCanvasPanelSlot>(button->Slot);
				if (slot != nullptr)
				{
					FVector2D pos;
					FAnchors anc;

					CalculatePosition(layers->Children[ind]->Layer->GetRect(), layers->Area, pos, anc);

					slot->SetAnchors(anc);
					slot->SetPosition(pos);
					slot->SetSize(FVector2D(layer->GetRect().Right - layer->GetRect().Left, layer->GetRect().Bottom - layer->GetRect().Top));
				}
			}

			FSlateBrush brush;
			brush.SetResourceObject(texture);
			brush.ImageSize = FVector2D(layer->GetRect().Left, layer->GetRect().Top);

			auto style = button->WidgetStyle;

			if (layer->AdditionalObjectType == PSDParser::LayerAdditionalObjectType::Normal)
			{
				style.SetNormal(brush);
			}

			if (layer->AdditionalObjectType == PSDParser::LayerAdditionalObjectType::Hovered)
			{
				style.SetHovered(brush);
			}

			if (layer->AdditionalObjectType == PSDParser::LayerAdditionalObjectType::Pressed)
			{
				style.SetPressed(brush);
			}

			button->SetStyle(style);
		}
		else if (layer->ObjectType == PSDParser::LayerObjectType::ProgressBar)
		{
			if (texture == nullptr) continue;

			UProgressBar* progressBar = nullptr;

			if (progressBars.Find(name) != nullptr)
			{
				progressBar = *progressBars.Find(name);
			}
			else
			{
				auto widget = userWidget->WidgetTree->FindWidget<UWidget>(*name);
				progressBar = userWidget->WidgetTree->FindWidget<UProgressBar>(*name);

				// if an widget which is not an progressBar exits, fail to create widgets.
				if (widget != nullptr && progressBar == nullptr)
				{
					continue;
				}

				if (progressBar == nullptr)
				{
					progressBar = userWidget->WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), *name);
					rootWidget->AddChild(progressBar);
				}

				progressBars.Add(name, progressBar);

				auto slot = Cast<UCanvasPanelSlot>(progressBar->Slot);
				if (slot != nullptr)
				{
					FVector2D pos;
					FAnchors anc;

					CalculatePosition(layers->Children[ind]->Layer->GetRect(), layers->Area, pos, anc);

					slot->SetAnchors(anc);
					slot->SetPosition(pos);
					slot->SetSize(FVector2D(layer->GetRect().Right - layer->GetRect().Left, layer->GetRect().Bottom - layer->GetRect().Top));
				}
			}

			FSlateBrush brush;
			brush.SetResourceObject(texture);
			brush.ImageSize = FVector2D(layer->GetRect().Left, layer->GetRect().Top);

			auto style = progressBar->WidgetStyle;

			if (layer->AdditionalObjectType == PSDParser::LayerAdditionalObjectType::Background)
			{
				style.SetBackgroundImage(brush);
			}

			if (layer->AdditionalObjectType == PSDParser::LayerAdditionalObjectType::Fill)
			{
				style.SetFillImage(brush);
			}

			if (layer->AdditionalObjectType == PSDParser::LayerAdditionalObjectType::Marquee)
			{
				style.SetMarqueeImage(brush);
			}

			progressBar->WidgetStyle = style;
		}
		else if (layer->ObjectType == PSDParser::LayerObjectType::Text)
		{
			auto widget = userWidget->WidgetTree->FindWidget<UWidget>(*name);
			auto text = userWidget->WidgetTree->FindWidget<UTextBlock>(*name);

			// if an widget which is not a text exits, fail to create widgets.
			if (widget != nullptr && text == nullptr)
			{
				continue;
			}

			if (text == nullptr)
			{
				text = userWidget->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), *name);
				rootWidget->AddChild(text);
			}

			auto str = FString((const wchar_t*)layer->GetText());
			text->Text = FText::FromString(str);

			auto slot = Cast<UCanvasPanelSlot>(text->Slot);
			if (slot != nullptr)
			{
				FVector2D pos;
				FAnchors anc;

				CalculatePosition(layers->Children[ind]->Layer->GetRect(), layers->Area, pos, anc);

				slot->SetAnchors(anc);
				slot->SetPosition(pos);
				slot->SetSize(FVector2D(layer->GetRect().Right - layer->GetRect().Left, layer->GetRect().Bottom - layer->GetRect().Top));
			}
		}
	}

	// assign buttons
	for (auto button : buttons)
	{
		auto style = button.Value->WidgetStyle;
		if (style.Hovered.GetResourceObject() == nullptr)
		{
			style.SetHovered(style.Normal);
		}

		if (style.Pressed.GetResourceObject() == nullptr)
		{
			style.SetPressed(style.Normal);
		}

		button.Value->SetStyle(style);
	}

	// assign progressBars
	for (auto progressBar : progressBars)
	{
		auto style = progressBar.Value->WidgetStyle;
		if (style.FillImage.GetResourceObject() == nullptr)
		{
			style.SetFillImage(style.BackgroundImage);
		}

		if (style.MarqueeImage.GetResourceObject() == nullptr)
		{
			style.SetMarqueeImage(style.BackgroundImage);
		}

		progressBar.Value->WidgetStyle = style;
	}
}

UObject* UPSD2UMGCacheFactory::FactoryCreateBinary(
	UClass* InClass,
	UObject* InParent,
	FName InName,
	EObjectFlags Flags,
	UObject* Context,
	const TCHAR* Type,
	const uint8*& Buffer,
	const uint8* BufferEnd,
	FFeedbackContext* Warn)
{
	auto document = PSDParser::Document::Create(Buffer, (int32_t)(BufferEnd - Buffer));
	if (document == nullptr) return nullptr;
	if (document->GetDepth() != 8) return nullptr;

	FString directoryPath = InParent->GetOutermost()->GetName() + TEXT("_psd/");

	UTextureFactory* textureFactory = NewObject<UTextureFactory>();
	textureFactory->AddToRoot();

	// create a cache
	UPSD2UMGCache* assetCache = NewObject<UPSD2UMGCache>(InParent, InClass, FName(InName), Flags);

	// create an widget
	FString widgetName = InName.ToString() + TEXT("_Widget");
	FString widgetPackageName = InParent->GetOutermost()->GetName() + TEXT("_Widget");

	auto userWidget = GetOrCreateWidget(widgetPackageName, widgetName, Flags);

	UCanvasPanel* rootWidget = Cast<UCanvasPanel>(userWidget->WidgetTree->RootWidget);
	if (rootWidget == nullptr && userWidget->WidgetTree->RootWidget != nullptr)
	{
		// if a root which is not a canvas exits, fail to create widgets.
		textureFactory->RemoveFromRoot();
		return nullptr;
	}

	if (rootWidget == nullptr)
	{
		rootWidget = userWidget->WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("Root"));
		userWidget->WidgetTree->RootWidget = rootWidget;
	}

	FVector2D canvasSize = FVector2D(document->GetWidth(), document->GetHeight());

	int32_t currentIndex = 0;
	auto layers = ParseLayers(document.get(), currentIndex);

	CreateWidgets(
		rootWidget,
		canvasSize,
		textureFactory,
		userWidget,
		InName,
		Flags,
		directoryPath,
		layers
	);

	textureFactory->RemoveFromRoot();

	userWidget->MarkPackageDirty();

	FAssetRegistryModule::AssetCreated(assetCache);
	assetCache->MarkPackageDirty();

	{
		if (!assetCache->AssetImportData)
		{
			assetCache->AssetImportData = NewObject<UAssetImportData>(assetCache, UAssetImportData::StaticClass());
		}

		assetCache->AssetImportData->Update(CurrentFilename);
	}

	return assetCache;
}

bool UPSD2UMGCacheFactory::CanReimport(UObject* Obj, TArray<FString>& OutFilenames)
{
	UPSD2UMGCache* asset = Cast<UPSD2UMGCache>(Obj);
	if (asset && asset->AssetImportData)
	{
		asset->AssetImportData->ExtractFilenames(OutFilenames);
		return true;
	}
	
	return false;
}

void UPSD2UMGCacheFactory::SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths)
{
	UPSD2UMGCache* asset = Cast<UPSD2UMGCache>(Obj);
	if (asset && ensure(NewReimportPaths.Num() == 1))
	{
		asset->AssetImportData->UpdateFilenameOnly(NewReimportPaths[0]);
	}
}

EReimportResult::Type UPSD2UMGCacheFactory::Reimport(UObject* Obj)
{
	UPSD2UMGCache* asset = Cast<UPSD2UMGCache>(Obj);
	if (!asset)
	{
		return EReimportResult::Failed;
	}

	const FString Filename = asset->AssetImportData->GetFirstFilename();

	if (!Filename.Len() || IFileManager::Get().FileSize(*Filename) == INDEX_NONE)
	{
		return EReimportResult::Failed;
	}

	EReimportResult::Type Result = EReimportResult::Failed;

	if (UFactory::StaticImportObject(
		asset->GetClass(),
		asset->GetOuter(),
		*asset->GetName(),
		RF_Public | RF_Standalone,
		*Filename,
		NULL,
		this))
	{
		if (asset->GetOuter())
		{
			asset->GetOuter()->MarkPackageDirty();
		}
		else
		{
			asset->MarkPackageDirty();
		}
		return EReimportResult::Succeeded;
	}

	return EReimportResult::Failed;
}
