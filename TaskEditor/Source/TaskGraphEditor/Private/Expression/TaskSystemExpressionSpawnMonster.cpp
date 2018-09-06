#pragma once

#include"Expression/TaskSystemExpressionSpawnMonster.h"
#include"Expression/TaskSystemExpressionOutput.h"

UTaskSystemExpressionSpawnMonster::UTaskSystemExpressionSpawnMonster(
	FObjectInitializer const& Initializer) : Super(Initializer) {
	FTaskSystemExpressionOutput Output;
	Output.OutputName = FName(TEXT(""));
	Output.OutputType = TOT_EventOutput;
	Outputs.Add(Output);
}



