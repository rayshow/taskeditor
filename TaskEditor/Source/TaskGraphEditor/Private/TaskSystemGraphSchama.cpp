#pragma once

#include"TaskSystemGraphSchema.h"
#include"TaskEditor.h"
#include"TaskSystemGraphNode.h"
#include"TaskSystemGraph.h"
#include"Expression/TaskSystemExpressionComment.h"
#include"Utils/TaskSystemUtils.h"
#include"AssetRegistryModule.h"

#define LOCTEXT_NAMESPACE "TaskGraphEditor"
const FName UTaskSystemGraphSchema::PC_TaskSubtarget(TEXT("TaskSubtarget")); 
const FName UTaskSystemGraphSchema::PC_Flow(TEXT("TaskFlow")); 
const FName UTaskSystemGraphSchema::PC_Event(TEXT("TaskEvent")); 
const FName UTaskSystemGraphSchema::PC_All(TEXT("TaskAll"));
const FName UTaskSystemGraphSchema::PSC_Test(TEXT("Test"));

const FLinearColor UTaskSystemGraphSchema::EventColor(0.69, 0.227, 0.596);
const FLinearColor UTaskSystemGraphSchema::FlowColor(1,1,1,1);



UEdGraphNode* FTaskSystemGraphSchemaAction_NewComment::PerformAction(class UEdGraph* ParentGraph,
	UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	return nullptr;
}

UEdGraphNode* FTaskSystemGraphSchemaAction_NewNode::PerformAction(
	UEdGraph* ParentGraph, UEdGraphPin* FromPin, 
	const FVector2D Location, bool bSelectNewNode)
{
	check(TaskSystemExpressionClass);

	auto NewExpression = FTaskEditor::CreateNewTaskSystemExpression(
		(UObject*)ParentGraph, TaskSystemExpressionClass, Location, bSelectNewNode, true);
	if (NewExpression)
	{
		NewExpression->GraphNode->AutowireNewNode(FromPin);
		return NewExpression->GraphNode;
	}
	return nullptr;
}


void FTaskConnectionDrawingPolicy::DetermineWiringStyle(UEdGraphPin* OutputPin, UEdGraphPin* InputPin, /*inout*/ FConnectionParams& Params)
{
	FConnectionDrawingPolicy::DetermineWiringStyle(OutputPin, InputPin, Params);
	if (OutputPin && InputPin &&
		OutputPin->PinType.PinCategory == UTaskSystemGraphSchema::PC_Event&&
		InputPin->PinType.PinCategory == UTaskSystemGraphSchema::PC_Event)
	{
		Params.WireColor = UTaskSystemGraphSchema::EventColor;
	}
	else {
		Params.WireColor = UTaskSystemGraphSchema::FlowColor;
		Params.WireThickness *= 2;
	}
}


UTaskSystemGraphSchema::UTaskSystemGraphSchema(FObjectInitializer const& Initializer)
	:UEdGraphSchema(Initializer) {}


void UTaskSystemGraphSchema::GetCommentAction(FGraphActionMenuBuilder& ActionMenuBuilder,
	const UEdGraph* CurrentGraph) const
{
	if (!ActionMenuBuilder.FromPin)
	{
		const bool bIsManyNodesSelected = CurrentGraph ? (FTaskEditor::GetNumberOfSelectedNodes(CurrentGraph) > 0) : false;
		const FText CommentDesc = LOCTEXT("CommentDesc", "New Comment");
		const FText MultiCommentDesc = LOCTEXT("MultiCommentDesc", "Create Comment from Selection");
		const FText CommentToolTip = LOCTEXT("CommentToolTip", "Creates a comment.");
		const FText MenuDescription = bIsManyNodesSelected ? MultiCommentDesc : CommentDesc;
		TSharedPtr<FTaskSystemGraphSchemaAction_NewComment> NewAction(
			new FTaskSystemGraphSchemaAction_NewComment(FText::GetEmpty(), MenuDescription, CommentToolTip, 0));
		ActionMenuBuilder.AddAction(NewAction);
	}
}

void UTaskSystemGraphSchema::GetPaletteActions(FGraphActionMenuBuilder& ActionMenuBuilder,
	UObject* Object, const FString& CategoryName)  const
{
	FTaskEditor::GetTaskSystemExpressionActions(ActionMenuBuilder, Object, PC_All);
	//GetCommentAction(ActionMenuBuilder);
}

void UTaskSystemGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	if (ContextMenuBuilder.CurrentGraph)
	{
		const UTaskSystemGraph* Graph = Cast<UTaskSystemGraph>(ContextMenuBuilder.CurrentGraph);
		UTaskObject* TaskObj = Graph->TaskObject;
		check(Graph);
		if (TaskObj)
		{
			// Run through all nodes and add any menu items they want to add
			Super::GetGraphContextActions(ContextMenuBuilder);

			FName PinCategory = PC_All;
			TArray<FName> Categoies;

			if (ContextMenuBuilder.FromPin)
			{
				PinCategory = ContextMenuBuilder.FromPin->PinType.PinCategory;
				EEdGraphPinDirection Dir = ContextMenuBuilder.FromPin->Direction;
				if (Dir == EGPD_Output)
				{
					if (PinCategory == PC_Flow || PinCategory == PC_TaskSubtarget)
					{
						Categoies.Add(PC_Flow);
						Categoies.Add(PC_TaskSubtarget);
					}
				}
				else {
					if (PinCategory == PC_Event)
					{
						Categoies.Add(PC_Event);
					}
				}
			}
			else {
				Categoies.Add(PC_All);
			}
			FTaskEditor::GetTaskSystemContextActions(ContextMenuBuilder, TaskObj, Categoies);
		}
	}
}

void UTaskSystemGraphSchema::GetContextMenuActions(const UEdGraph* CurrentGraph, const UEdGraphNode* InGraphNode, const UEdGraphPin* InGraphPin, class FMenuBuilder* MenuBuilder, bool bIsDebugging) const
{
	//FTaskEditor::GetTaskSystemContextActions(CurrentGraph, InGraphNode, InGraphPin, MenuBuilder, bIsDebugging);
	UEdGraphSchema::GetContextMenuActions(CurrentGraph, InGraphNode, InGraphPin, MenuBuilder, bIsDebugging);
}

void UTaskSystemGraphSchema::GetTaskSystemFunctionActions(FGraphActionMenuBuilder& ActionMenuBuilder) const
{
	// Get type of dragged pin
	uint32 FromPinType = 0;
	if (ActionMenuBuilder.FromPin)
	{
		FromPinType = GetTaskSystemValueType(ActionMenuBuilder.FromPin);
	}

	// Load the asset registry module
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
}

static bool NotCompatible(FName A, FName B)
{
	if (A == UTaskSystemGraphSchema::PC_Event &&
		(B == UTaskSystemGraphSchema::PC_Flow ||
			B == UTaskSystemGraphSchema::PC_TaskSubtarget))
		return true;
	return false;
}

static bool CanConnect(UEdGraphPin* From, UEdGraphPin* To)
{
	if (From == To ||
		From->GetOwningNode() == To->GetOwningNode() ||
		From->Direction == To->Direction ||
		NotCompatible(From->PinType.PinCategory, To->PinType.PinCategory) ||
		NotCompatible(To->PinType.PinCategory, From->PinType.PinCategory)) return false;
	return true;
}

bool UTaskSystemGraphSchema::TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const{
	if (CanConnect(A, B))
	{
		return UEdGraphSchema::TryCreateConnection(A, B);
	}
		
	return false;
}


uint32 UTaskSystemGraphSchema::GetTaskSystemValueType(const UEdGraphPin* Pin)
{
	if (Pin->Direction == EGPD_Output)
	{
		UTaskSystemGraphNode* OwningNode = CastChecked<UTaskSystemGraphNode>(Pin->GetOwningNode());
		return OwningNode->GetOutputType(Pin);
	}
	else
	{
		UTaskSystemGraphNode_Base* OwningNode = CastChecked<UTaskSystemGraphNode_Base>(Pin->GetOwningNode());
		return OwningNode->GetInputType(Pin);
	}
}


FLinearColor UTaskSystemGraphSchema::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	if (PinType.PinCategory == PC_Event)
	{
		return EventColor;
	}
	
	return FlowColor;
}


FConnectionDrawingPolicy* UTaskSystemGraphSchema::CreateConnectionDrawingPolicy(
	int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor,
	const FSlateRect& InClippingRect, class FSlateWindowElementList& InDrawElements, 
	class UEdGraph* InGraphObj) const
{
	return new FTaskConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements);
}



#undef LOCTEXT_NAMESPACE