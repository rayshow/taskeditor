#pragma once

#include"SBranchPin.h"
#include"SEventPin.h"
#include"SEventTextPin.h"
#include"SSelectNPCPin.h"
#include"TaskSystemGraphSchema.h"

class FPinWidgetFactory
{
public:
	static TSharedPtr<SGraphPin> CreatePinWidget(UEdGraphPin* InPin)
	{
		if (InPin->PinType.PinCategory == UTaskSystemGraphSchema::PC_NPC)
		{
			return SNew(SSelectNPCPin, InPin);
		}
		else if (InPin->PinType.PinCategory == UTaskSystemGraphSchema::PC_Branch)
		{
			return SNew(SBranchPin, InPin);
		}
		else if (InPin->PinType.PinCategory == UTaskSystemGraphSchema::PC_Event)
		{
			if(InPin->Direction == EGPD_Input) return SNew(SEventPin, InPin);
			else return SNew(SEventTextPin, InPin);
		}
		check(false);
		return nullptr;
	}
};