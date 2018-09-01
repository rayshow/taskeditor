// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;
class FExtender;
class UStaticMesh;
struct FAssetData;
struct FAutoGenBentMesh;


class FBentMaterialAutoGenModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	//Scan All BentNormal Mesh
	void ScanAllBentNormalMeshes();
	
	//Add a Context Menu for Bent-normal Mesh
	TSharedRef<FExtender> GenerateBentNormalMeshContextMenu(const TArray<FAssetData>&);

	//filter Bent-normal Meshes from selected Assets
	void SelectBentNormalMeshes(const TArray<FAssetData>& SelectedFiles);

private:

	void AddToolbarExtension(FToolBarBuilder& Builder);
	void AddMenuExtension(FMenuBuilder& Builder);

private: 
	TSharedPtr<class FUICommandList> PluginCommands;
	TSharedPtr<class FUICommandList> ContextCommands;
	TArray<FAutoGenBentMesh>         NeedPrecessMeshes;
	TArray<UStaticMesh*>             NoBentNormalMeshes;
};