#include"HeightAlignTool/Public/HeightAlignTool.h"
#include"LevelEditor.h"
#include"ModuleManager.h"
#include"CoreMinimal.h"
#include"MultiBoxBuilder.h"
#include "Dialogs/Dialogs.h"
#include"Engine.h"
#include"Editor.h"
#include"Landscape.h"
#include"LandscapeComponent.h"
#include "MessageLog/Public/MessageLogModule.h"
#include "MessageLog/Public/IMessageLogListing.h"
#include "MessageLog.h"
#include "ScopedTransaction.h"
#include "Components/PrimitiveComponent.h"
#include "Input/SCheckBox.h"
#define LOCTEXT_NAMESPACE "FHeightAlignTool"


static void AlertErrorMessage(EMessageSeverity::Type LV, FString&& Msg)
{
	auto box = SNew(SCheckBox);
	

	auto& MessageLogModule = FModuleManager::LoadModuleChecked<FMessageLogModule>("MessageLog");
	FMessageLog MessageLog("HeightAlign Tool");

	auto Message = FTokenizedMessage::Create(LV);

	Message->AddToken(FTextToken::Create(FText::FromString(Msg)));
	MessageLog.AddMessage(Message);

	MessageLog.Notify(FText::FromString(Msg), LV, false);
	MessageLogModule.OpenMessageLog("HeightAlign Tool");
}

static void ClearErrorMessage()
{
	auto& MessageLogModule = FModuleManager::LoadModuleChecked<FMessageLogModule>("MessageLog");
	auto LogListing = MessageLogModule.GetLogListing("BentNormal Tool");
	LogListing->ClearMessages();
}

static void AlignActor(AActor* Actor, const FName& TraceTag)
{
	Actor->SetFlags(EObjectFlags::RF_Transactional);

	bool HitTerrain = false;
	FVector Pos; FRotator Rot;
	Actor->GetActorEyesViewPoint(Pos, Rot);
	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(TraceTag, true);
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.bTraceAsyncScene = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;

	//trace down
	while (!HitTerrain)
	{
		FHitResult Hit(ForceInit);
		GWorld->LineTraceSingleByChannel(Hit, Pos, Pos - FVector(0, 0, 100000000), ECollisionChannel::ECC_WorldStatic, RV_TraceParams);

		if (!Hit.bBlockingHit) {
			//not hit
			break;
		}

		if (nullptr != Hit.Actor->GetComponentByClass(ULandscapeComponent::StaticClass())) {
			//hit terrain
			HitTerrain = true;
			Actor->EditorApplyTranslation(FVector(0, 0, -1)*Hit.Distance, false, false, false);
			break;
		}
		else {
			//not hit terrain but other object
			RV_TraceParams.AddIgnoredActor(Hit.Actor.Get());
		}
	}

	while (!HitTerrain)
	{
		FHitResult Hit(ForceInit);
		GWorld->LineTraceSingleByChannel(Hit, Pos, Pos + FVector(0, 0, 100000000), ECollisionChannel::ECC_WorldStatic, RV_TraceParams);

		if (!Hit.bBlockingHit) {
			//not hit
			AlertErrorMessage(EMessageSeverity::Error, FString::Printf(TEXT("%s Not On Landscape."), *Actor->GetName()));
			break;
		}

		if (nullptr != Hit.Actor->GetComponentByClass(ULandscapeComponent::StaticClass())) {
			//hit terrain
			HitTerrain = true;
			Actor->EditorApplyTranslation(FVector(0, 0, 1)*Hit.Distance, false, false, false);
			break;
		}
		else {
			//not hit terrain but other object
			RV_TraceParams.AddIgnoredActor(Hit.Actor.Get());
		}
	}

	if (HitTerrain)
	{
		Actor->Modify();
	}
}


static void AlignComponent(AActor* Actor, UStaticMeshComponent* Comp, const FName& TraceTag)
{
	Comp->SetFlags(EObjectFlags::RF_Transactional);

	bool HitTerrain = false;
	FVector Pos = Comp->GetComponentLocation();
	
	
	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(TraceTag, true);
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.bTraceAsyncScene = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;

	//trace down
	while (!HitTerrain)
	{
		FHitResult Hit(ForceInit);
		GWorld->LineTraceSingleByChannel(Hit, Pos, Pos - FVector(0, 0, 100000000), ECollisionChannel::ECC_WorldStatic, RV_TraceParams);

		if (!Hit.bBlockingHit) {
			//not hit
			break;
		}

		if (nullptr != Hit.Actor->GetComponentByClass(ULandscapeComponent::StaticClass())) {
			//hit terrain
			HitTerrain = true;
			Comp->AddWorldOffset(FVector(0, 0, -Hit.Distance));
			break;
		}
		else {
			//not hit terrain but other object
			RV_TraceParams.AddIgnoredActor(Hit.Actor.Get());
		}
	}

	while (!HitTerrain)
	{
		FHitResult Hit(ForceInit);
		GWorld->LineTraceSingleByChannel(Hit, Pos, Pos + FVector(0, 0, 100000000), ECollisionChannel::ECC_WorldStatic, RV_TraceParams);

		if (!Hit.bBlockingHit) {
			//not hit
			AlertErrorMessage(EMessageSeverity::Error, FString::Printf(TEXT("%s Not On Landscape."), *Actor->GetName()));
			break;
		}

		if (nullptr != Hit.Actor->GetComponentByClass(ULandscapeComponent::StaticClass())) {
			//hit terrain
			HitTerrain = true;
			Comp->AddWorldOffset(FVector(0, 0, Hit.Distance));
			break;
		}
		else {
			//not hit terrain but other object
			RV_TraceParams.AddIgnoredActor(Hit.Actor.Get());
		}
	}

	if (HitTerrain)
	{
		Comp->Modify();
	}
}


static void ProcessAsset(const TArray<AActor*> Actors)
{
	const FName TraceTag("MyTraceTag");
	
	GWorld->DebugDrawTraceTag = TraceTag;

	ClearErrorMessage();
	for (int i = 0; i < Actors.Num(); ++i)
	{ 
		AlignActor(Actors[i], TraceTag);
	}
}

static void ProcessAssetWithComponent(const TArray<AActor*> Actors)
{
	const FName TraceTag("MyTraceTag");

	GWorld->DebugDrawTraceTag = TraceTag;

	ClearErrorMessage();
	for (int i = 0; i < Actors.Num(); ++i)
	{
		auto& Actor = Actors[i];
		auto MeshCompArr = Actor->GetComponentsByClass(UStaticMeshComponent::StaticClass());	
		if (MeshCompArr.Num() > 1) {
			for (auto ActorComp : MeshCompArr) {
				auto PrimComp = Cast<UStaticMeshComponent>(ActorComp);
				if (PrimComp) {
					AlignComponent(Actor, PrimComp, TraceTag);
				}
			}
		}
		else {
			AlignActor(Actor, TraceTag);
		}
	}
}

void FHeightAlignTool::StartupModule()
{
	// see actor as a integral object
	auto& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	auto& Extenders = LevelEditorModule.GetAllLevelViewportContextMenuExtenders();
	Extenders.Add(FLevelEditorModule::FLevelViewportMenuExtender_SelectedActors::CreateLambda([]
		(const TSharedRef<FUICommandList> InCommandList, const TArray<AActor*> InActors) {
		
		TSharedPtr<FExtender> Extender = MakeShareable(new FExtender);
		Extender->AddMenuExtension("ActorControl", EExtensionHook::First,MakeShareable(new FUICommandList),
			FMenuExtensionDelegate::CreateLambda([InActors](FMenuBuilder& Builder) {
			Builder.AddMenuEntry(
				LOCTEXT("TerrainHeightAlign", "Terrain Height Align"),
				LOCTEXT("TerrainHeightAlignTT", "TerrainHeightAlign Tooltip"),
				FSlateIcon(),
				FExecuteAction::CreateLambda([InActors]() { 

				FScopedTransaction Trans(LOCTEXT("HeightAlign_Transaction", "Height Align Actors"));
				ProcessAsset(InActors);
			}));
		}));
		return Extender.ToSharedRef();
	}));

	//Align each static_mesh component
	Extenders.Add(FLevelEditorModule::FLevelViewportMenuExtender_SelectedActors::CreateLambda([]
	(const TSharedRef<FUICommandList> InCommandList, const TArray<AActor*> InActors) {

		TSharedPtr<FExtender> Extender = MakeShareable(new FExtender);
		Extender->AddMenuExtension("ActorControl", EExtensionHook::First, MakeShareable(new FUICommandList),
			FMenuExtensionDelegate::CreateLambda([InActors](FMenuBuilder& Builder) {
			Builder.AddMenuEntry(
				LOCTEXT("TerrainHeightAlignWC", "Terrain Height Align(with Component)"),
				LOCTEXT("TerrainHeightAlignWCTT", "TerrainHeightAlign Tooltip WC"),
				FSlateIcon(),
				FExecuteAction::CreateLambda([InActors]() {


				FScopedTransaction Trans(LOCTEXT("HeightAlign_Transaction", "Height Align Actors"));

				ProcessAssetWithComponent(InActors);
			}));
		}));
		return Extender.ToSharedRef();
	}));


	//register a message error log tab
	FMessageLogModule& MessageLogModule = FModuleManager::LoadModuleChecked<FMessageLogModule>("MessageLog");
	FMessageLogInitializationOptions InitOptions;
	InitOptions.bShowPages = true;
	MessageLogModule.RegisterLogListing("HeightAlign Tool", LOCTEXT("HeightAlign ToolLL", "HeightAlign Tool"), InitOptions);

}


void FHeightAlignTool::ShutdownModule()
{
}


IMPLEMENT_MODULE(FHeightAlignTool, HeightAlignTool)

#undef LOCTEXT_NAMESPACE