#pragma once

#include"ObjectMacros.h"
#include"EdGraph/EdGraphSchema.h"
#include "ConnectionDrawingPolicy.h"
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


class FTaskConnectionDrawingPolicy : public FConnectionDrawingPolicy
{
public:
	FTaskConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID,
		float InZoomFactor, const FSlateRect& InClippingRect,
		FSlateWindowElementList& InDrawElements) :
		FConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor,
			InClippingRect, InDrawElements) {}

	virtual void DetermineWiringStyle(UEdGraphPin* OutputPin, UEdGraphPin* InputPin, /*inout*/ FConnectionParams& Params) override;
};


UCLASS(MinimalAPI)
class UTaskSystemGraphSchema : public UEdGraphSchema
{
public:
	GENERATED_UCLASS_BODY()

	//PinType
	static const FName PC_Flow;        //White and Bold
	static const FName PC_TaskSubtarget; //White and Bold
	static const FName PC_Event;         //Red and Solid
	static const FName PC_All;

	static const FName PSC_Test;

	static const FLinearColor EventColor;
	static const FLinearColor FlowColor;

	void GetCommentAction(FGraphActionMenuBuilder& ActionMenuBuilder,
		const UEdGraph* CurrentGraph = nullptr) const;

	// Generate Menu For PalletteActions
	void GetPaletteActions(FGraphActionMenuBuilder& ActionMenuBuilder, 
		UObject* Object, const FString& CategoryName) const;
	
	// Generate Menu For Pin Point to Context Menu
	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;

	//Generate Menu for Pin
	virtual void GetContextMenuActions(const UEdGraph* CurrentGraph, const UEdGraphNode* InGraphNode, const UEdGraphPin* InGraphPin, class FMenuBuilder* MenuBuilder, bool bIsDebugging) const override;


	void GetTaskSystemFunctionActions(FGraphActionMenuBuilder& ActionMenuBuilder) const;

	bool TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const override;

	virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT("Not implemented by this schema"));
	}

	virtual FLinearColor GetPinTypeColor(const FEdGraphPinType& PinType) const override;
	virtual class FConnectionDrawingPolicy* CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, class FSlateWindowElementList& InDrawElements, class UEdGraph* InGraphObj) const override;



	static uint32 GetTaskSystemValueType(const UEdGraphPin* MaterialPin);
};