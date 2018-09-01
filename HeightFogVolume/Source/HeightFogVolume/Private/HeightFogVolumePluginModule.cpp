// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "HeightFogVolumePluginModule.h"
#include "Editor/PlacementMode/Public/IPlacementModeModule.h"
#include "ActorFactories/ActorFactoryBoxVolume.h"
#include "HeightFogVolume.h"

#define LOCTEXT_NAMESPACE "FHeightFogVolumePluginModule"

void CreateFactoryForHeightVolume() {
		for (TObjectIterator<UClass> ObjectIt; ObjectIt; ++ObjectIt)
		{
			UClass* TestClass = *ObjectIt;
			if (!TestClass->HasAnyClassFlags(CLASS_Abstract) && TestClass->IsChildOf(UActorFactoryVolume::StaticClass()))
			{
				UActorFactory* NewFactory = NewObject<UActorFactory>(GetTransientPackage(), TestClass);
				check(NewFactory);
				NewFactory->NewActorClass = AHeightFogVolume::StaticClass();
				GEditor->ActorFactories.Add(NewFactory);
			}
		}
}

void FHeightFogVolumePluginModule::StartupModule()
{
	auto& PlacementModule = IPlacementModeModule::Get();
	auto Info = PlacementModule.GetRegisteredPlacementCategory(FBuiltInPlacementCategories::Volumes());
	CreateFactoryForHeightVolume();
	auto Factory = GEditor->FindActorFactoryByClassForActorClass(UActorFactoryBoxVolume::StaticClass(),
		AHeightFogVolume::StaticClass());
	PlacementModule.RegisterPlaceableItem(Info->UniqueHandle,
		MakeShareable(new FPlaceableItem(Factory, FAssetData(AHeightFogVolume::StaticClass()))));
}

void FHeightFogVolumePluginModule::ShutdownModule()
{
	
}

IMPLEMENT_MODULE(FHeightFogVolumePluginModule, HeightFogVolumePlugin)

#undef LOCTEXT_NAMESPACE
	
