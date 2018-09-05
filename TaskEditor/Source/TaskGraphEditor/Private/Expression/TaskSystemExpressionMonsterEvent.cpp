#pragma once

#include"Expression/TaskSystemExpressionMonsterEvent.h"
#include"Expression/TaskSystemExpressionOutput.h"

UTaskSystemExpressionMonsterEvent::UTaskSystemExpressionMonsterEvent(
	FObjectInitializer const& Initializer) : Super(Initializer) {
	FTaskSystemExpressionOutput Output;
	Output.OutputName = FName(TEXT("招怪ID"));
	Output.OutputType = TOT_EventOutput;
	Outputs.Add(Output);
}



