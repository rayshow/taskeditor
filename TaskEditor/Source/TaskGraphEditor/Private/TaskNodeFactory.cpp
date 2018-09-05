
#include"TaskNodeFactory.h"
#include"TaskSystemGraphNode.h"
#include"CoreMinimal.h"
#include"Casts.h"
#include"Pins/STaskLinePin.h"
#include"Pins/SSelectNPCPin.h"
#include"Pins/SEventPin.h"
#include"nodes/STaskNode.h"
#include"TaskSystemGraphSchema.h"
#include"ConnectionDrawingPolicy.h"
#include"EdGraphUtilities.h"
#include"EdGraph/EdGraphSchema.h"

TSharedPtr<SGraphNode> FTaskNodeFactory::CreateNodeWidget(UEdGraphNode* InNode)
{
	if (UTaskSystemGraphNode_Base* BaseNode = Cast<UTaskSystemGraphNode_Base>(InNode))
	{
		if (UTaskSystemGraphNode* Node = Cast<UTaskSystemGraphNode>(InNode))
		{
			return SNew(STaskNodeDefault, InNode);
		}
		check(false);
	}
	else {
		TSharedPtr<SGraphNode> NodeCreatedResult = InNode->CreateVisualWidget();
		if (NodeCreatedResult.IsValid())
		{
			return NodeCreatedResult;
		}
		check(false);
	}
	return nullptr;
}

TSharedPtr<SGraphPin> FTaskNodeFactory::CreatePinWidget(UEdGraphPin* InPin)
{
	if (InPin->PinType.PinCategory == UTaskSystemGraphSchema::PC_TaskThread)
	{
		return SNew(SSelectNPCPin, InPin);
	}
	else if (InPin->PinType.PinCategory == UTaskSystemGraphSchema::PC_Event)
	{
		return SNew(SEventPin, InPin);
	}
	check(false);
	return nullptr;
}


FConnectionDrawingPolicy* 
FTaskNodeFactory::CreateConnectionPolicy(
	const UEdGraphSchema* Schema, int32 InBackLayerID, int32 InFrontLayerID,
	float ZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements,
	UEdGraph* InGraphObj)
{
	FConnectionDrawingPolicy* ConnectionDrawingPolicy;

	// First give the schema a chance to provide the connection drawing policy
	ConnectionDrawingPolicy = Schema->CreateConnectionDrawingPolicy(
		InBackLayerID, InFrontLayerID, ZoomFactor, InClippingRect, InDrawElements, InGraphObj);


	// If neither the schema nor the factory provides a policy, try the hardcoded ones
	//@TODO: Fold all of this code into registered factories for the various schemas!
	if (!ConnectionDrawingPolicy)
	{
			ConnectionDrawingPolicy = new FConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, ZoomFactor, InClippingRect, InDrawElements);
	}
	check(ConnectionDrawingPolicy);
	return ConnectionDrawingPolicy;
}

