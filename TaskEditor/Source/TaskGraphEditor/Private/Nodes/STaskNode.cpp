// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.


#include "Nodes/STaskNode.h"


void STaskNodeDefault::Construct(const FArguments& InArgs, UEdGraphNode* InNode)
{
	this->GraphNode = InNode;

	this->SetCursor(EMouseCursor::CardinalCross);

	this->UpdateGraphNode();
}
