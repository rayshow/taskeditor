#pragma once

#include"TaskSystem.h"
#include"TaskSystemGraphNode.h"
#include"TaskSystemGraphSchema.h"


class FTaskUtils
{
public:
	static UTaskSystem* GetResourceFromNode(UEdGraphNode* Node)
	{
		UObject* Outer = Cast<UTaskSystemGraphSchema>( Node->GetOuter() );
		while (Outer) {
			Outer = Outer->GetOuter();
			if (Outer->GetClass()->IsChildOf<UTaskSystem>())
			{
				return Cast<UTaskSystem>(Outer);
			}
		}
		return nullptr;
	}

	

	
};