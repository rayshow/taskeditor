#include"TaskSystemGraph.h"
#include"EdGraph/EdGraphPin.h"
#include"TaskSystemExpression.h"
#include"TaskSystemExpressionComment.h"
#include"TaskSystemGraphNode.h"
#include"TaskSystemExpressionInput.h"


UTaskSystemGraph::UTaskSystemGraph(FObjectInitializer const& Initializer) 
	:UEdGraph(Initializer) {}

void UTaskSystemGraph::LinkTaskSystemExpressionsFromGraph() const
{
	// Use GraphNodes to make Material Expression Connections
	TArray<UEdGraphPin*> InputPins;
	TArray<UEdGraphPin*> OutputPins;

	for (int32 NodeIndex = 0; NodeIndex < Nodes.Num(); ++NodeIndex)
	{
		if (UTaskSystemGraphNode* GraphNode = Cast<UTaskSystemGraphNode>(Nodes[NodeIndex]))
		{
			// Need to be sure that we are changing the expression before calling modify -
			// triggers a rebuild of its preview when it is called
			UTaskSystemExpression* Expression = GraphNode->Expression;
			bool bModifiedExpression = false;

			//if (Expression->MaterialExpressionEditorX != GraphNode->NodePosX
			//	|| Expression->MaterialExpressionEditorY != GraphNode->NodePosY
			//	|| Expression->Desc != GraphNode->NodeComment)
			//{
			//	bModifiedExpression = true;

			//	Expression->Modify();

			//	// Update positions and comments
			//	Expression->MaterialExpressionEditorX = GraphNode->NodePosX;
			//	Expression->MaterialExpressionEditorY = GraphNode->NodePosY;
			//	Expression->Desc = GraphNode->NodeComment;
			//}

			GraphNode->GetInputPins(InputPins);
			const TArray<FTaskSystemExpressionInput*> ExpressionInputs = Expression->GetInputs();
			checkf(InputPins.Num() == ExpressionInputs.Num(), TEXT("Mismatched inputs for '%s'"), *Expression->GetFullName());
			for (int32 PinIndex = 0; PinIndex < InputPins.Num() && PinIndex < ExpressionInputs.Num(); ++PinIndex)
			{
				FTaskSystemExpressionInput* ExpressionInput = ExpressionInputs[PinIndex];
				if (InputPins[PinIndex]->LinkedTo.Num() > 0)
				{

					UTaskSystemGraphNode* ConnectedNode = CastChecked<UTaskSystemGraphNode>(
						InputPins[PinIndex]->LinkedTo[0]->GetOwningNode());
					ConnectedNode->GetOutputPins(OutputPins);

					// Work out the index of the connected pin
					for (int32 OutPinIndex = 0; OutPinIndex < OutputPins.Num(); ++OutPinIndex)
					{
						if (OutputPins[OutPinIndex] == InputPins[PinIndex]->LinkedTo[0])
						{
							if (ExpressionInput->OutputIndex != OutPinIndex 
							  || ExpressionInput->Expression != ConnectedNode->Expression)
							{
								if (!bModifiedExpression)
								{
									bModifiedExpression = true;
									Expression->Modify();
								}
								ConnectedNode->Expression->Modify();
								ExpressionInput->Connect(OutPinIndex, ConnectedNode->Expression);
							}
							break;
						}
					}
				}
				else if (ExpressionInput->Expression)
				{
					if (!bModifiedExpression)
					{
						bModifiedExpression = true;
						Expression->Modify();
					}
					ExpressionInput->Expression = NULL;
				}
			}
		}
		//else if (UTaskSystemGraphNodeComment* CommentNode = Cast<UTaskSystemGraphNodeComment>(Nodes[NodeIndex]))
		//{
		//	UMaterialExpressionComment* Comment = CommentNode->MaterialExpressionComment;

		//	if (Comment->MaterialExpressionEditorX != CommentNode->NodePosX
		//		|| Comment->MaterialExpressionEditorY != CommentNode->NodePosY
		//		|| Comment->Text != CommentNode->NodeComment
		//		|| Comment->SizeX != CommentNode->NodeWidth
		//		|| Comment->SizeY != CommentNode->NodeHeight
		//		|| Comment->CommentColor != CommentNode->CommentColor)
		//	{
		//		Comment->Modify();

		//		// Update positions and comments
		//		Comment->MaterialExpressionEditorX = CommentNode->NodePosX;
		//		Comment->MaterialExpressionEditorY = CommentNode->NodePosY;
		//		Comment->Text = CommentNode->NodeComment;
		//		Comment->SizeX = CommentNode->NodeWidth;
		//		Comment->SizeY = CommentNode->NodeHeight;
		//		Comment->CommentColor = CommentNode->CommentColor;
		//	}
		//}
	}
}

UTaskSystemGraphNode* UTaskSystemGraph::AddExpression(UTaskSystemExpression* Expression, bool bUserInvoked)
{
	UTaskSystemGraphNode* NewNode = NULL;
	if (Expression)
	{
			Modify();
			FGraphNodeCreator<UTaskSystemGraphNode> NodeCreator(*this);
			if (bUserInvoked)
			{
				NewNode = NodeCreator.CreateUserInvokedNode();
			}
			else
			{
				NewNode = NodeCreator.CreateNode(false);
			}
			NewNode->Expression = Expression;
			//NewNode->RealtimeDelegate = RealtimeDelegate;
			//NewNode->MaterialDirtyDelegate = MaterialDirtyDelegate;
			Expression->GraphNode = NewNode;
			NodeCreator.Finalize();
	}
	return NewNode;
}