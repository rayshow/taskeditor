#pragma once

#include"ObjectMacros.h"
#include"CoreMinimal.h"
#include"Factories/Factory.h"
#include"TaskSystem.generated.h"

class UTaskGroup;
class UTaskModule;

const FString TEXT_TaskSystem{ TEXT("TaskSystem") };
const FString TEXT_TaskModule{ TEXT("TaskModule") };
const FString TEXT_TaskGroup{ TEXT("TaskGroup") };
const FString TEXT_TaskSelf{ TEXT("TaskSelf") };

UCLASS(BlueprintType, MinimalAPI)
class UTaskObject : public UObject
{
public:
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere)
	FName  Name;

	UPROPERTY()
	class UTaskSystemGraph* Graph;

	UPROPERTY()
	TArray<class UTaskSystemExpression*> Expressions;

	UPROPERTY()
	TArray<class UTaskSystemExpressionComment*> Comments;

	UPROPERTY()
	bool Initialized;

	void Initialize();

	virtual void PrivateInitialize();

	virtual void AddChild(UTaskObject* Child) {}

	virtual const FString* GetFilterCategory() { return nullptr; };
};

UCLASS(BlueprintType, MinimalAPI)
class UTask : public UTaskObject
{
public:
	GENERATED_UCLASS_BODY()

	UTaskGroup* Group;

	UTaskModule* Module;

	virtual const FString* GetFilterCategory() override
	{
		return &TEXT_TaskSelf;
	}
};

UCLASS(BlueprintType, MinimalAPI)
class UTaskGroup : public UTaskObject
{
public:
	GENERATED_UCLASS_BODY()
	
	UPROPERTY()
	TArray<UTaskObject*> Children;
	
	virtual void AddChild(UTaskObject* Child) override
	{
		if (Child->GetClass()->IsChildOf<UTaskGroup>() ||
			Child->GetClass()->IsChildOf<UTask>())
		{
			Children.Add(Child);
		}
	}

	virtual const FString* GetFilterCategory() override
	{
		return &TEXT_TaskGroup;
	}

};

// As Top Group, no Father Group
UCLASS(BlueprintType, MinimalAPI)
class UTaskModule : public UTaskGroup
{
public:
	GENERATED_UCLASS_BODY()


	virtual void PrivateInitialize() override;

	virtual void AddChild(UTaskObject* Child) override {
		UTaskGroup* ChildGroup = Cast<UTaskGroup>(Child);
		if (ChildGroup)
		{
			Children.Add(ChildGroup);
		}
	}

	virtual const FString* GetFilterCategory() override
	{
		return &TEXT_TaskModule;
	}
};

UCLASS(BlueprintType, MinimalAPI)
class UTaskSystem : public UTaskObject
{
public:
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere)
	TArray<UTaskModule*> Modules;

	virtual const FString* GetFilterCategory() override
	{
		return &TEXT_TaskSystem;
	}
};

UCLASS()
class UTaskSystemFactory : public UFactory
{
	GENERATED_UCLASS_BODY()

	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent,
		FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};


UCLASS()
class UTaskModuleFactory : public UFactory
{
	GENERATED_UCLASS_BODY()

	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent,
		FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};

