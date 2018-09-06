#pragma once

#include"ObjectMacros.h"
#include"EdGraph/EdGraphSchema.h"
#include"TaskSystemGraphSchema.generated.h"

USTRUCT()
struct FTaskSystemGraphSchemaAction_NewComment : public FEdGraphSchemaAction
{
public:
	GENERATED_USTRUCT_BODY();


	// Simple type info
	static FName StaticGetTypeId() { static FName Type("FMaterialGraphSchemaAction_NewComment"); return Type; }
	virtual FName GetTypeId() const override { return StaticGetTypeId(); }

	FTaskSystemGraphSchemaAction_NewComment()
		: FEdGraphSchemaAction()
	{}

	FTaskSystemGraphSchemaAction_NewComment(FText InNodeCategory,
		FText InMenuDesc, FText InToolTip, const int32 InGrouping)
		: FEdGraphSchemaAction(MoveTemp(InNodeCategory), MoveTemp(InMenuDesc), MoveTemp(InToolTip), InGrouping) {}
	 
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph,
		UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
 
};


/** Action to add an expression node to the graph */
USTRUCT()
struct FTaskSystemGraphSchemaAction_NewNode : public FEdGraphSchemaAction
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY()
	class UClass* TaskSystemExpressionClass;

	static FName StaticGetTypeId() { static FName Type("FTaskSystemGraphSchemaAction_NewNode"); return Type; }
	virtual FName GetTypeId() const override { return StaticGetTypeId(); }

	FTaskSystemGraphSchemaAction_NewNode()
		: FEdGraphSchemaAction()
		, TaskSystemExpressionClass(nullptr)
	{}

	FTaskSystemGraphSchemaAction_NewNode(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping, FText InKeywords)
		: FEdGraphSchemaAction(MoveTemp(InNodeCategory), MoveTemp(InMenuDesc), MoveTemp(InToolTip), InGrouping, MoveTemp(InKeywords))
		, TaskSystemExpressionClass(nullptr)
	{}

	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;

};



UCLASS(MinimalAPI)
class UTaskSystemGraphSchema : public UEdGraphSchema
{
public:
	GENERATED_UCLASS_BODY()

	//PinType
	static const FName PC_Branch;     //White and Bold
	static const FName PC_NPC;
	static const FName PC_Event;      //Red and Solid

	void GetPaletteActions(FGraphActionMenuBuilder& ActionMenuBuilder, const FString& CategoryName, bool bMaterialFunction) const;
	void GetCommentAction(FGraphActionMenuBuilder& ActionMenuBuilder, const UEdGraph* CurrentGraph = nullptr) const;
	void GetTaskSystemFunctionActions(FGraphActionMenuBuilder& ActionMenuBuilder) const;

	virtual bool TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const override {
		UEdGraphSchema::TryCreateConnection(A, B);
		return true;
	}

	virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT("Not implemented by this schema"));
	}

	virtual FLinearColor GetPinTypeColor(const FEdGraphPinType& PinType) const override {
		return FLinearColor(1, 1, 1);
	}
	static uint32 GetTaskSystemValueType(const UEdGraphPin* MaterialPin);
};