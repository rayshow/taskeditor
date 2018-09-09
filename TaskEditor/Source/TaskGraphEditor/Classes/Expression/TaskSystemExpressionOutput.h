#pragma once

#include"CoreMinimal.h"
#include"TaskSystemGraphSchema.h"

UENUM()
enum TaskSystemExpressionOutputType {
	TOT_EventOutput = 0,
	TOT_NPCExit,
	TOT_NPCExit_Test,
	TOT_BranchExit
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
		switch (OutputType) {
		case TOT_EventOutput:
			return UTaskSystemGraphSchema::PC_Event;
		case TOT_NPCExit:
		case TOT_NPCExit_Test:
			return UTaskSystemGraphSchema::PC_TaskSubtarget;
		case TOT_BranchExit:
			return UTaskSystemGraphSchema::PC_Flow;
		default:
			check(false);
		}
		return NAME_None;
	}

	FName OutputSubEnumToName()
	{
		switch (OutputType) {
		case TOT_NPCExit_Test:
			return UTaskSystemGraphSchema::PSC_Test;
		default:
			return NAME_None;
		}
	}
};
