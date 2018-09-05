#pragma once

#include"CoreMinimal.h"
#include"ObjectMacros.h"
#include"TaskSystemGraph.generated.h"


class UTaskSystemGraphNode;

DECLARE_DELEGATE(FSetTaskSystemDirty);

UCLASS()
class UTaskSystemGraph: public UEdGraph
{
public:

	GENERATED_UCLASS_BODY()

	//Main Resource Object
	class UTaskObject* TaskObject;

	// Mark Task System Dirty
	FSetTaskSystemDirty SetTaskSystemDirty;

	void LinkTaskSystemExpressionsFromGraph() const;

	UTaskSystemGraphNode* AddExpression(UTaskSystemExpression* Expression, bool bUserInvoked);
};