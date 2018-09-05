#pragma once

#include"CoreMinimal.h"
#include"TaskSystemGraphSchema.h"

UENUM()
enum TaskSystemExpressionOutputType {
	TOT_EventOutput = 0,
};

USTRUCT(noexport)
struct FTaskSystemExpressionOutput
{
	UPROPERTY()
	FName	OutputName;

	UPROPERTY()
	TEnumAsByte<TaskSystemExpressionOutputType> OutputType;

	FTaskSystemExpressionOutput() {}

	FName  OutputEnumToName() {
		switch (TOT_EventOutput) {
		case TOT_EventOutput:
			return UTaskSystemGraphSchema::PC_Event;
		default:
			check(false);
		}
		return NAME_None;
	}
};
