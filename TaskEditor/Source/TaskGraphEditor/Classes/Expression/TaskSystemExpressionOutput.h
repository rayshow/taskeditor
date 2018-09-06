#pragma once

#include"CoreMinimal.h"
#include"TaskSystemGraphSchema.h"

UENUM()
enum TaskSystemExpressionOutputType {
	TOT_EventOutput = 0,
	TOT_NPCExit,
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
			return UTaskSystemGraphSchema::PC_NPC;
		case TOT_BranchExit:
			return UTaskSystemGraphSchema::PC_Branch;
		default:
			check(false);
		}
		return NAME_None;
	}
};
