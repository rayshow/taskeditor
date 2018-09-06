#pragma once

#include"Expression/TaskSystemExpressionWeather.h"
#include"Expression/TaskSystemExpressionOutput.h"


const FName UTaskSystemExpressionWeather::SubCatgory("Weather");

UTaskSystemExpressionWeather::UTaskSystemExpressionWeather(FObjectInitializer const& Initializer)
	: Super(Initializer) {
	FTaskSystemExpressionOutput Output;
	Output.OutputName = FName(TEXT(""));
	Output.OutputType = TOT_EventOutput;
	Outputs.Add(Output);
}

