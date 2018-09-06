
#include"TaskNodeFactory.h"
#include"TaskSystemGraphNode.h"
#include"CoreMinimal.h"
#include"Casts.h"
#include"TaskSystemGraphSchema.h"
#include"Nodes/NodeWidgetFactory.h"
#include"Pins/PinWidgetFactory.h"
#include"ConnectionDrawingPolicy.h"
#include"EdGraphUtilities.h"
#include"EdGraph/EdGraphSchema.h"

TSharedPtr<SGraphNode> FTaskNodeFactory::CreateNodeWidget(UEdGraphNode* InNode)
{
	return FNodeWidgetFactory::CreateNodeWidget(InNode);
}

TSharedPtr<SGraphPin> FTaskNodeFactory::CreatePinWidget(UEdGraphPin* InPin)
{
	return FPinWidgetFactory::CreatePinWidget(InPin);
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

