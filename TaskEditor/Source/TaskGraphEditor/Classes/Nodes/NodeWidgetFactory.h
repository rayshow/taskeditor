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
			UClass* Clazz = Node->Expression->GetClass();
			if (Clazz->IsChildOf<UTaskSystemExpressionSubtarget>())
			{
				return SNew(SSubtargetNode, InNode);
			}
			else if ( Clazz->IsChildOf<UTaskSystemExpressionSpawnMonster>() ||
					Clazz->IsChildOf<UTaskSystemExpressionWeather>() )
			{
				return SNew(SSpawnMonsterNode, InNode);
			}
			else if (auto Expr = Cast<UTaskSystemExpression1In2Dialog>(Node->Expression))
			{
				return SNew(SDialogNode, InNode);
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