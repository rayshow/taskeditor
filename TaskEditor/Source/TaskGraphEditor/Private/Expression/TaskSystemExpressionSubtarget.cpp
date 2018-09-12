#pragma once

#include"Expression/TaskSystemExpressionSubtarget.h"
#include"Expression/TaskSystemExpressionOutput.h"

UTaskSystemExpressionSubtarget::UTaskSystemExpressionSubtarget(FObjectInitializer const& Initializer)
	: Super(Initializer) {
	FTaskSystemExpressionOutput Output;
	Output.OutputName = FName(TEXT("交NPC"));
	Output.OutputType = TOT_NPCExit;
	Outputs.Add(Output);

	//Input
	Input.InputName = FName(TEXT("接NPC"));
	Input.InputType = TIT_NPCEntry;

	//Event
	BeforeAccess.InputName = FName(TEXT("接之前"));
	BeforeAccess.InputType = TIT_EventInput;

	AfterAccess.InputName = FName(TEXT("接之前"));
	AfterAccess.InputType = TIT_EventInput;

	BeforePost.InputName = FName(TEXT("交之前"));
	BeforePost.InputType = TIT_EventInput;

	AfterPost.InputName = FName(TEXT("交之后"));
	AfterPost.InputType = TIT_EventInput;
}

