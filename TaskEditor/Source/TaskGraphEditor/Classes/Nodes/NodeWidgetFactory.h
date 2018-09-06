#pragma once

#include"SSubtargetNode.h"
#include"SSpawnMonsterNode.h"
#include"TaskSystemGraphNode.h"
#include"SDialogNode.h"
#include"Expression/TaskSystemExpressionSubtarget.h"
#include"Expression/TaskSystemExpressionSpawnMonster.h"
#include"Expression/TaskSystemExpressionDialog.h"

class FNodeWidgetFactory
{
public:
	static TSharedPtr<SGraphNode> CreateNodeWidget(UEdGraphNode* InNode)
	{
		if (UTaskSystemGraphNode* Node = Cast<UTaskSystemGraphNode>(InNode))
		{
			if (auto Expr = Cast<UTaskSystemExpressionSubtarget>(Node->Expression))
			{
				return SNew(SSubtargetNode, InNode);
			}
			else if (auto Expr = Cast<UTaskSystemExpressionSpawnMonster>(Node->Expression))
			{
				return SNew(SSpawnMonsterNode, InNode);
			}
			else if (auto Expr = Cast<UTaskSystemExpression1In2Dialog>(Node->Expression))
			{
				return SNew(SDialogNode, InNode);
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
};