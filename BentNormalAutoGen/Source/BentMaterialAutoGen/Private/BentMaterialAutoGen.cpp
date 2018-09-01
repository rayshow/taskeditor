// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "BentMaterialAutoGen.h"
#include "BentMaterialAutoGenStyle.h"
#include "BentMaterialAutoGenCommands.h"
#include "Misc/MessageDialog.h"
#include"Editor.h"
#include "IContentBrowserSingleton.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "ContentBrowserModule.h"
#include "LevelEditor.h"
#include "AssetRegistryModule.h"
#include "FileHelpers.h"
#include "Materials/MaterialInstance.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Materials/MaterialExpressionBentNormalCustomOutput.h"
#include "Dialogs/Dialogs.h"
#include "Notifications/SNotificationList.h"
#include "Notifications/NotificationManager.h"
#include "Misc/ScopedSlowTask.h"
#include "MessageLog/Public/MessageLogModule.h"
#include "MessageLog/Public/IMessageLogListing.h"
#include "MessageLog.h"

static const FName BentMaterialAutoGenTabName("BentMaterialAutoGen");

#define LOCTEXT_NAMESPACE "FBentMaterialAutoGenModule"

struct FAutoGenBentMesh {
	UStaticMesh*    Mesh;
	TArray<uint32>  NeededGenMatIndice;
};


void FBentMaterialAutoGenModule::StartupModule()
{
	// do register
	FBentMaterialAutoGenStyle::Initialize();
	FBentMaterialAutoGenStyle::ReloadTextures();
	FBentMaterialAutoGenCommands::Register();

	//create a menu button to Scan All Mesh with BentNormal and Process
	PluginCommands = MakeShareable(new FUICommandList);
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	TSharedRef<FExtender> MenuExtender(new FExtender());
	MenuExtender->AddMenuExtension(
		TEXT("EditMain"),
		EExtensionHook::After,
		PluginCommands.ToSharedRef(),
		FMenuExtensionDelegate::CreateLambda([this](FMenuBuilder& MenuBuilder) {
		MenuBuilder.AddMenuEntry(
			LOCTEXT("BentNormalAllGen", "BentNormal Material All Generate"),
			LOCTEXT("DemoToolsTooltip", "List of tools"),
			FSlateIcon(),
			FExecuteAction::CreateRaw(this, &FBentMaterialAutoGenModule::ScanAllBentNormalMeshes));
	}));
	LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);


	//register a context menu of Mesh Asset if It contain BentNormal resource
	FContentBrowserModule& ContentBrowserModule = FModuleManager::GetModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
	FContentBrowserMenuExtender_SelectedAssets AssetExtender;
	AssetExtender.BindRaw(this, &FBentMaterialAutoGenModule::GenerateBentNormalMeshContextMenu);
	ContentBrowserModule.GetAllAssetViewContextMenuExtenders().Add(AssetExtender);

	//register a message error log tab
	FMessageLogModule& MessageLogModule = FModuleManager::LoadModuleChecked<FMessageLogModule>("MessageLog");
	FMessageLogInitializationOptions InitOptions;
	InitOptions.bShowPages = true;
	MessageLogModule.RegisterLogListing("BentNormal Tool", LOCTEXT("BentNormalToolLogLabel", "BentNormal Tool"), InitOptions);
}

void FBentMaterialAutoGenModule::ShutdownModule()
{
	FBentMaterialAutoGenStyle::Shutdown();
	FBentMaterialAutoGenCommands::Unregister();
}

static void AlertErrorMessage(FString&& Msg)
{
	auto& MessageLogModule = FModuleManager::LoadModuleChecked<FMessageLogModule>("MessageLog");
	FMessageLog MessageLog("BentNormal Tool");

	auto Message = FTokenizedMessage::Create(EMessageSeverity::Error);

	Message->AddToken(FTextToken::Create(FText::FromString(Msg)));
	MessageLog.AddMessage(Message);

	MessageLog.Notify(FText::FromString(Msg), EMessageSeverity::CriticalError, false);
	MessageLogModule.OpenMessageLog("BentNormal Tool");
}

static void AlertInfoMessage(FString&& Msg)
{
	auto& MessageLogModule = FModuleManager::LoadModuleChecked<FMessageLogModule>("MessageLog");
	FMessageLog MessageLog("BentNormal Tool");

	auto Message = FTokenizedMessage::Create(EMessageSeverity::Info);

	Message->AddToken(FTextToken::Create(FText::FromString(Msg)));
	MessageLog.AddMessage(Message);

	MessageLog.Notify(FText::FromString(Msg), EMessageSeverity::Info, false);
	MessageLogModule.OpenMessageLog("BentNormal Tool");
}


static void ClearErrorMessage()
{
	auto& MessageLogModule = FModuleManager::LoadModuleChecked<FMessageLogModule>("MessageLog");
	auto LogListing = MessageLogModule.GetLogListing("BentNormal Tool");
	LogListing->ClearMessages();
}

static void ProcessingMeshes(TArray<FAutoGenBentMesh>& NeedProcessMeshes)
{
	check(NeedProcessMeshes.Num() > 0);

	auto& Registries = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();

	TArray<UPackage*> PacksToSave;
	FScopedSlowTask SlowTask(NeedProcessMeshes.Num() + 1, LOCTEXT("ProcessBentNormalMesh", "Generate Bent Normal Materials"));
	SlowTask.MakeDialog();
	SlowTask.EnterProgressFrame();

	//Prograss Process
	for (auto& Item : NeedProcessMeshes)
	{
		auto Mesh = Item.Mesh;
		SlowTask.EnterProgressFrame();

		bool MultiSlot = Item.NeededGenMatIndice.Num() > 1;
		for (int32 i = 0; i < Item.NeededGenMatIndice.Num(); ++i) 
		{
			auto Mat = Mesh->GetMaterial(i);
			if (Mat)
			{
				auto MatInst = Cast<UMaterialInstance>(Mat);
				if (MatInst) {

					//Get Directory of Mesh
					int32 Index = -1;
					Mesh->GetPathName().FindLastChar(TEXT('/'), Index);
					FString MeshDir = Mesh->GetPathName().Left(Index);
						
					//load BentNormal Texture
					FString BentNormalTexName = Mesh->GetName() + "_btn";
					FString BentNormalTexPath = MeshDir + TEXT("/TextureBentNor/") + BentNormalTexName + TEXT(".") + BentNormalTexName;
					FAssetData BentNormalTexAsset = Registries.GetAssetByObjectPath(FName(*(BentNormalTexPath)));
					UObject* BentNormalTexObj = BentNormalTexAsset.GetAsset();
					UTexture* BtnTex = Cast<UTexture>(BentNormalTexObj);
					if (!BtnTex)
					{
						AlertErrorMessage(FString::Printf(TEXT("  %s Slot %d Bent-Normal texture not found."),  *Mesh->GetFullName(), i));
						continue;
					}

					//load OA Texture
					FString AOTexName = Mesh->GetName() + "_ao";
					FString AOTexPath = MeshDir + TEXT("/TextureBentNor/") + AOTexName + TEXT(".") + AOTexName;
					FAssetData AOTexAsset = Registries.GetAssetByObjectPath(FName(*(AOTexPath)));
					UObject* AOTexObj = AOTexAsset.GetAsset();
					UTexture* AOTex = Cast<UTexture>(AOTexObj);
					if (!AOTex)
					{
						AlertErrorMessage(FString::Printf(TEXT(" %s Slot %d AO texture not found."), *Mesh->GetFullName(), i));
						continue;
					}

					FString BentNormalMatName = Mesh->GetName() + "_BentNor";
					if (MultiSlot) {
						BentNormalMatName = BentNormalMatName + FString::Printf(TEXT("_0%d"), i+1);
					}
					FString BentNormalMatPath = MeshDir + "/MaterialBentNor/" + BentNormalMatName;

					UPackage* Package = CreatePackage(NULL, *BentNormalMatPath);

					auto MatWithBentNormal = NewObject<UMaterialInstanceConstant>(Package, 
						FName(*BentNormalMatName),
						RF_Public | RF_Standalone | RF_Transactional);

					MatWithBentNormal->Parent = MatInst;

					MatWithBentNormal->SetTextureParameterValueEditorOnly(FName("Bent"), BtnTex);
					MatWithBentNormal->SetTextureParameterValueEditorOnly(FName("LowAO"), AOTex);
					MatWithBentNormal->PostLoad();

					Mesh->StaticMaterials[i].MaterialInterface = MatWithBentNormal;
						
					Package->MarkPackageDirty();
					PacksToSave.Add(Package);

					AlertInfoMessage(FString::Printf(TEXT(" Processed %s Slot %d BentNormal."), *Mesh->GetFullName(), i));
				}
			}
		}
	}
	//Save Generate Files
	FEditorFileUtils::PromptForCheckoutAndSave(PacksToSave, true, false);
}


void FBentMaterialAutoGenModule::SelectBentNormalMeshes(const TArray<FAssetData>& SelectedFiles)
{
	NeedPrecessMeshes.Empty();
	NoBentNormalMeshes.Empty();
	

	if (SelectedFiles.Num() > 0) {
		FScopedSlowTask SlowTask(SelectedFiles.Num() + 1, LOCTEXT("CollectSelectedMesh", "Collecting Selected Mesh Data"));
		SlowTask.MakeDialog();
		SlowTask.EnterProgressFrame(); 

		for (auto& Pack : SelectedFiles) 
		{
			UStaticMesh* Mesh = Cast<UStaticMesh>(Pack.GetAsset());
			SlowTask.EnterProgressFrame();
			if (Mesh)
			{
				TArray<uint32> CurrentNeedIndice;
				bool NeedProcess = false;
				for (int32 i = 0; i < Mesh->StaticMaterials.Num(); ++i)
				{
					UMaterialInterface* MeshMaterial = Mesh->GetMaterial(i);
					if (MeshMaterial)
					{
						UMaterialInstance* MInst = Cast<UMaterialInstance>(MeshMaterial);
						if (MInst && MInst->Parent) 
						{
							auto PMat = Cast<UMaterial>(MInst->Parent);
							if (PMat && PMat->GetMaterial()) 
							{
								auto Mat = PMat->GetMaterial();
								for (auto& Expr : Mat->Expressions) 
								{
									//Contain Bent Normal Output
									if (Expr->GetName().StartsWith("MaterialExpressionBentNormalCustomOutput")) {
										NeedProcess = true;
										CurrentNeedIndice.Add(i);
									}
								}
							}
						}
					}
				}
				if (NeedProcess) NeedPrecessMeshes.Add({ Mesh, CurrentNeedIndice });
				else {
					NoBentNormalMeshes.Add(Mesh);
				}
			}
		}
	}
}
 
TSharedRef<FExtender> FBentMaterialAutoGenModule::GenerateBentNormalMeshContextMenu(const TArray<FAssetData>& InAssets)
{  
	SelectBentNormalMeshes(InAssets);
	
	// Create a Menu Entry after Asset Special Context Menu Items 
	TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
	if (NeedPrecessMeshes.Num() > 0) {
		ContextCommands = MakeShareable(new FUICommandList);

		// if Select Assets exists Meshes Contain Bent - normal Material
		MenuExtender->AddMenuExtension(
			"GetAssetActions", EExtensionHook::After, ContextCommands, 
			FMenuExtensionDelegate::CreateLambda([this](class FMenuBuilder& MenuBuilder) 
			{
					
					//if Exists Meshes with BentNormal Material
					if (NeedPrecessMeshes.Num() > 0)
					{
						ClearErrorMessage();
						// Display Menu Entry
						MenuBuilder.AddMenuEntry(
							LOCTEXT("BentNormalContext", "BentNormal Material Generate"),
							LOCTEXT("DemoToolsContextTooltip", "Tooltips"),
							FSlateIcon(), FExecuteAction::CreateLambda([this]() 
						{ 

							// Some Mesh selected is NoBentNormal
							if (NoBentNormalMeshes.Num()>0) {
								// output error info
								for (int i = 0; i < NoBentNormalMeshes.Num(); ++i)
								{
									AlertErrorMessage(FString::Printf(TEXT(" %s Is Not BentNormal Mesh."), *NoBentNormalMeshes[i]->GetFullName()));
								}

								// Ask to Continue
								if (EAppReturnType::No == OpenMsgDlgInt(EAppMsgType::YesNo,
									LOCTEXT("BentNormalMeshError", "Some Meshes has No BentNormal Material, Continue Processing ?"),
									LOCTEXT("BentNormalMeshErrorTT", "Continue ?"))) {
									// Not Continue
									return;
								}
							}
							// Continue Process this Meshes
							ProcessingMeshes(NeedPrecessMeshes);
						}));
					}
			}));
	}
	return MenuExtender.ToSharedRef();
}

void FBentMaterialAutoGenModule::ScanAllBentNormalMeshes()
{
	auto& Registries = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
	NeedPrecessMeshes.Empty();
	NoBentNormalMeshes.Empty();

	//find mother material 
	TSet<UMaterial*> MotherMaterialSet;
	{
		TArray<FAssetData> Packs;
		FARFilter filter;
		filter.PackagePaths.Add("/Game/BaseMaterial/SceneMatBase");
		filter.ClassNames.Add(UMaterial::StaticClass()->GetFName());

		Registries.GetAssets(filter, Packs);

		if (Packs.Num() > 0)
		{
			FScopedSlowTask SlowTask(Packs.Num() + 1, LOCTEXT("CollectBaseBentMaterial", "Collecting Bent Normal Materials"));
			SlowTask.MakeDialog();
			SlowTask.EnterProgressFrame();
			for (auto& Pack : Packs) {
				UMaterial* Mat = Cast<UMaterial>(Pack.GetAsset());
				SlowTask.EnterProgressFrame();
				if (Mat) {
					for (auto& Expr : Mat->Expressions) {
						//auto BentExpr = Cast<UMaterialExpressionBentNormalCustomOutput>(Expr);
						//Contain Bent Normal Output
						if (Expr->GetName().StartsWith("MaterialExpressionBentNormalCustomOutput")) {
							MotherMaterialSet.Add(Mat, nullptr);
							continue;
						}
					}
				}
			}
		}
	}

	//find mesh contain material derived from mother material
	{
		TArray<FAssetData> Packs2;
		FARFilter filter2;
		filter2.PackagePaths.Add("/Game/Scene");
		filter2.ClassNames.Add(UStaticMesh::StaticClass()->GetFName());
		filter2.bRecursivePaths = true;
		Registries.GetAssets(filter2, Packs2);

		if (Packs2.Num() > 0)
		{
			FScopedSlowTask SlowTask(Packs2.Num() + 1, LOCTEXT("CollectMesh", "Collecting Meshes"));
			SlowTask.MakeDialog();
			SlowTask.EnterProgressFrame();

			for (auto& Pack : Packs2) {
				UStaticMesh* Mesh = Cast<UStaticMesh>(Pack.GetAsset());
				SlowTask.EnterProgressFrame();

				bool NeedProcess = false;
				if (Mesh) {
					TArray<uint32> CurrentNeedIndice;
					for (int32 i = 0; i < Mesh->StaticMaterials.Num(); ++i)
					{
						UMaterialInterface* MeshMaterial = Mesh->GetMaterial(i);
						if (MeshMaterial) {
							UMaterialInstance* MInst = Cast<UMaterialInstance>(MeshMaterial);
							if (MInst && MInst->Parent) {
								auto PMat = Cast<UMaterial>(MInst->Parent);
								if (PMat && MotherMaterialSet.Find(PMat)) {
									CurrentNeedIndice.Add(i);
									NeedProcess = true;
								}
							}
						}
					}
					if (NeedProcess) NeedPrecessMeshes.Add({ Mesh, CurrentNeedIndice });
				}
			}
		}
	}

	
	if (NeedPrecessMeshes.Num() > 0)
	{
		ClearErrorMessage();
		ProcessingMeshes(NeedPrecessMeshes);
	}
}

void FBentMaterialAutoGenModule::AddMenuExtension(FMenuBuilder& Builder)
{
	Builder.AddMenuEntry(FBentMaterialAutoGenCommands::Get().PluginAction);
}

void FBentMaterialAutoGenModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
	Builder.AddToolBarButton(FBentMaterialAutoGenCommands::Get().PluginAction);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FBentMaterialAutoGenModule, BentMaterialAutoGen)