// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Toolkits/IToolkitHost.h"
#include "AssetTypeActions_Base.h"
#include "../Classes/TaskSystem.h"
#include "AssetTypeActions_Base.h"

class FMenuBuilder;

class FAssetTypeActions_TaskSystem : public FAssetTypeActions_Base
{
public:
	// IAssetTypeActions Implementation
	virtual FText GetName() const override { return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_TaskSystem", "任务主表"); }
	virtual FColor GetTypeColor() const override { return FColor(176, 58, 152, 255); }
	virtual UClass* GetSupportedClass() const override { return UTaskSystem::StaticClass(); }
	virtual bool HasActions(const TArray<UObject*>& InObjects) const override { return false; }
	virtual void GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder) override {};
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects,
		TSharedPtr<class IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override
	{
		EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

		for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
		{
			auto TaskSystem = Cast<UTaskSystem>(*ObjIt);
			if (TaskSystem != NULL)
			{
				ITaskEditorModule& Module = FModuleManager::LoadModuleChecked<ITaskEditorModule>("TaskGraphEditor");
				Module.CreateTaskEditor(Mode, EditWithinLevelEditor, TaskSystem);
			}
		}
	}
	virtual uint32 GetCategories() override { return EAssetTypeCategories::Basic; }
};


class FAssetTypeActions_TaskModule : public FAssetTypeActions_Base
{
public:
	// IAssetTypeActions Implementation
	virtual FText GetName() const override { return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_TaskModule", "任务分表"); }
	virtual FColor GetTypeColor() const override { return FColor(176, 58, 152,255); }
	virtual UClass* GetSupportedClass() const override { return UTaskModule::StaticClass(); }
	virtual bool HasActions(const TArray<UObject*>& InObjects) const override { return false; }
	virtual void GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder) override {};
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects,
		TSharedPtr<class IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override
	{
		EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

		for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
		{
			auto TaskModule = Cast<UTaskModule>(*ObjIt);
			if (TaskModule != NULL)
			{
				ITaskEditorModule& Module = FModuleManager::LoadModuleChecked<ITaskEditorModule>("TaskGraphEditor");
				Module.CreateTaskEditor(Mode, EditWithinLevelEditor, TaskModule);
			}
		}
	}
	virtual uint32 GetCategories() override { return EAssetTypeCategories::Basic; }
};
