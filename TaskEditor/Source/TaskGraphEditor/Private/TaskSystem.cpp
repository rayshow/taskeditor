#include"Classes/TaskSystem.h"
#include"TaskSystemGraph.h"
#include"TaskSystemGraphSchema.h"
#include "BlueprintEditorUtils.h"


///////////////////////////////////////////////////////////////////////////////////
//// Task base

UTaskObject::UTaskObject(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), Initialized(false)
{
	
}

void UTaskObject::Initialize()
{
	if (!Initialized)
	{
		PrivateInitialize();
		Initialized = true;
	}
}

void UTaskObject::PrivateInitialize()
{
	if (!Graph) {
		Graph = CastChecked<UTaskSystemGraph>(FBlueprintEditorUtils::CreateNewGraph(
			this, NAME_None, UTaskSystemGraph::StaticClass(), UTaskSystemGraphSchema::StaticClass()));
		Graph->TaskObject = this;
	}
	check(Graph);
}

///////////////////////////////////////////////////////////////////////////////////
//// Task Obj

UTask::UTask(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}


///////////////////////////////////////////////////////////////////////////////////
//// TaskGroup Obj

UTaskGroup::UTaskGroup(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}


///////////////////////////////////////////////////////////////////////////////////
//// TaskModule Obj

UTaskModule::UTaskModule(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UTaskModule::PrivateInitialize()
{
	Super::PrivateInitialize();
	if (!Initialized)
	{
		UTaskGroup* MainlineGroup = NewObject<UTaskGroup>(this);
		MainlineGroup->Name = FName(TEXT("主线任务"));
		MainlineGroup->Initialize();

		UTaskGroup* SidelineGroup = NewObject<UTaskGroup>(this);
		SidelineGroup->Name = FName(TEXT("支线任务"));
		SidelineGroup->Initialize();

		Children.Add(MainlineGroup);
		Children.Add(SidelineGroup);

		Name = FName(*this->GetName());
	}
}



///////////////////////////////////////////////////////////////////////////////////
//// Task System Obj

UTaskSystem::UTaskSystem(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}



///////////////////////////////////////////////////////////////////////////////////
//// Task System Factory

UTaskSystemFactory::UTaskSystemFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UTaskSystem::StaticClass();
}

UObject* UTaskSystemFactory::FactoryCreateNew(UClass* Class, UObject* InParent,
	FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UTaskSystem* NewObjectAsset = NewObject<UTaskSystem>(InParent, Class, Name, Flags);
	return NewObjectAsset;
}

///////////////////////////////////////////////////////////////////////////////////
//// Task Module Factory

UTaskModuleFactory::UTaskModuleFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UTaskModule::StaticClass();
}

UObject* UTaskModuleFactory::FactoryCreateNew(UClass* Class, UObject* InParent,
	FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UTaskModule* NewObjectAsset = NewObject<UTaskModule>(InParent, Class, Name, Flags);
	return NewObjectAsset;
}