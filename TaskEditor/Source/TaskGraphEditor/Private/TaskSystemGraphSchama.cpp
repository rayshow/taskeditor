#pragma once

#include"TaskSystemGraphSchema.h"
#include"TaskSystemExpressionComment.h"
#include"TaskEditor.h"
#include"TaskSystemGraphNode.h"
#include"AssetRegistryModule.h"

#define LOCTEXT_NAMESPACE "TaskGraphEditor"


UEdGraphNode* 
FTaskSystemGraphSchemaAction_NewComment::PerformAction(class UEdGraph* ParentGraph,
	UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	return nullptr;
}

UEdGraphNode* FTaskSystemGraphSchemaAction_NewNode::PerformAction(
	UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	check(TaskSystemExpressionClass);

	UTaskSystemExpression* NewExpression = FTaskEditor::CreateNewTaskSystemExpression(
		ParentGraph, TaskSystemExpressionClass, Location, bSelectNewNode, true);
	if (NewExpression)
	{
		NewExpression->GraphNode->AutowireNewNode(FromPin);
		return NewExpression->GraphNode;
	}
	return nullptr;
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
	const FString& CategoryName, bool bTaskSystemFunction)  const
{
	
	FTaskEditor::GetTaskSystemExpressionActions(ActionMenuBuilder, bTaskSystemFunction);
	GetCommentAction(ActionMenuBuilder);


	/*if (CategoryName != TEXT("Functions"))
	{
	}
	if (CategoryName != TEXT("Expressions"))
	{
		GetTaskSystemFunctionActions(ActionMenuBuilder);
	}*/
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

	// Collect a full list of assets with the specified class
	/*TArray<FAssetData> AssetDataList;
	AssetRegistryModule.Get().GetAssetsByClass(UMaterialFunction::StaticClass()->GetFName(), AssetDataList);*/

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


#undef LOCTEXT_NAMESPACE