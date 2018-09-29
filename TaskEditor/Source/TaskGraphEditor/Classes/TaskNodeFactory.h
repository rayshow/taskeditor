#pragma once

#include"NodeFactory.h"

class FTaskNodeFactory
{
public:

	static TSharedPtr<SGraphNode> CreateNodeWidget(UEdGraphNode* InNode);
	
	static TSharedPtr<SGraphPin> CreatePinWidget(UEdGraphPin* InPin);

	static FConnectionDrawingPolicy* CreateConnectionPolicy(
		const UEdGraphSchema* Schema, int32 InBackLayerID, int32 InFrontLayerID,
		float ZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements,
		UEdGraph* InGraphObj);
};



class FTaskNodeGraphFactory : public FGraphNodeFactory
{
public:
	// ~ Start Override FGraphNodeFactory 
	virtual TSharedPtr<SGraphNode> CreateNodeWidget(UEdGraphNode* InNode) override
	{ 
		return FTaskNodeFactory::CreateNodeWidget(InNode);
	}

	virtual TSharedPtr<SGraphPin> CreatePinWidget(UEdGraphPin* InPin) override
	{
		return FTaskNodeFactory::CreatePinWidget(InPin);
	}

	virtual FConnectionDrawingPolicy* CreateConnectionPolicy(
		const UEdGraphSchema* Schema, int32 InBackLayerID, int32 InFrontLayerID,
		float ZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements,
		UEdGraph* InGraphObj) override
	{
		return FTaskNodeFactory::CreateConnectionPolicy(Schema, InBackLayerID,
			InFrontLayerID, ZoomFactor, InClippingRect, InDrawElements, InGraphObj);
	}
	// ~ End Override FGraphNodeFactory 
};