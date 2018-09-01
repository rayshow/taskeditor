#pragma once

#include"ObjectMacros.h"
#include"CoreMinimal.h"
#include"Object.h"
#include"Factories/Factory.h"
#include"EdGraph/EdGraphNode.h"
#include"SharedPointer.h"
#include"TaskSystem.generated.h"


class UTaskGroup;
class UTaskModule;

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


};

UCLASS(BlueprintType, MinimalAPI)
class UTask : public UTaskObject
{
public:
	GENERATED_UCLASS_BODY()

	UTaskGroup* Group;

	UTaskModule* Module;
};

UCLASS(BlueprintType, MinimalAPI)
class UTaskGroup : public UTaskObject
{
public:
	GENERATED_UCLASS_BODY()
	
	UPROPERTY()
	TArray<UTaskObject*> Children;
	
	virtual void AddChild(UTaskObject* Child) override{
		if (Child->GetClass()->IsChildOf<UTaskGroup>()
			|| Child->GetClass()->IsChildOf<UTask>())
		{
			Children.Add(Child);
		}
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
};

UCLASS(BlueprintType, MinimalAPI)
class UTaskSystem : public UTaskObject
{
public:
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere)
	TArray<UTaskModule*> Modules;
};

UCLASS()
class UTaskSystemFactory : public UFactory
{
	GENERATED_UCLASS_BODY()

	// UFactory interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent,
		FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};


UCLASS()
class UTaskModuleFactory : public UFactory
{
	GENERATED_UCLASS_BODY()

	// UFactory interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent,
		FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};

