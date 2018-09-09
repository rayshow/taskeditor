#pragma once

#include"SBranchPin.h"
#include"SEventPin.h"
#include"SSpawnMonsterPin.h"
#include"SWeatherPin.h"
#include"SSelectNPCPin.h"
#include"SInputNPCIdPin.h"
#include"Expression/TaskSystemExpressionWeather.h"
#include"Expression/TaskSystemExpressionSpawnMonster.h"
#include"TaskSystemGraphSchema.h"

class FPinWidgetFactory
{
public:
	static TSharedPtr<SGraphPin> CreatePinWidget(UEdGraphPin* InPin)
	{
		if (InPin->PinType.PinCategory == UTaskSystemGraphSchema::PC_TaskSubtarget)
		{
			if (InPin->PinType.PinSubCategory == UTaskSystemGraphSchema::PSC_Test)
			{
				return SNew(SInputNpcIdPin, InPin);
			}
			else {
				return SNew(SSelectNPCPin, InPin);
			}
		}
		else if (InPin->PinType.PinCategory == UTaskSystemGraphSchema::PC_Flow)
		{
			return SNew(SBranchPin, InPin);
		}
		else if (InPin->PinType.PinCategory == UTaskSystemGraphSchema::PC_Event)
		{
			if (InPin->Direction == EGPD_Input)
			{
				return SNew(SEventPin, InPin);
			}
			else {
				check(InPin->PinType.PinSubCategoryObject.Get());
				UClass* Clazz = InPin->PinType.PinSubCategoryObject->GetClass();
				if (Clazz->IsChildOf<UTaskSystemExpressionSpawnMonster>())
					return SNew(SSpawnMonsterPin, InPin);
				else if (Clazz->IsChildOf<UTaskSystemExpressionWeather>())
					return SNew(SWeatherPin, InPin);
				else {
					check(false);
				}
			}
		}
		check(false);
		return nullptr;
	}
};