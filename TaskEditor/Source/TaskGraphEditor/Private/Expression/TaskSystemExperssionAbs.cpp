#pragma once

#include"Expression/TaskSystemExpressionAbs.h"
#include"TaskSystemExpressionOutput.h"

UTaskSystemExpressionAbs::UTaskSystemExpressionAbs(FObjectInitializer const& Initializer)
	: Super(Initializer) {
	FTaskSystemExpressionOutput Output;
	Output.OutputName = FName("Out");
	Input.InputName = FName("In");
	Outputs.Add(Output);
}

