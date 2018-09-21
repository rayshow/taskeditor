#pragma once

#include"Expression/TaskSystemExpressionSubtask.h"
#include"Expression/TaskSystemExpressionOutput.h"

UTaskSystemExpressionSubtask_Common::UTaskSystemExpressionSubtask_Common(
	FObjectInitializer const& Initializer)
	: Super(Initializer) {

	//Input
	Input.InputName = FName(TEXT("接NPC"));
	Input.InputType = TIT_NPCEntry_Test;

	//Event
	BeforeAccess.InputName = FName(TEXT("接之前"));
	BeforeAccess.InputType = TIT_EventInput;

	AfterAccess.InputName = FName(TEXT("接之前"));
	AfterAccess.InputType = TIT_EventInput;
}


UTaskSystemExpressionSubtask::UTaskSystemExpressionSubtask(
	FObjectInitializer const& Initializer)
	: Super(Initializer) 
{
	FTaskSystemExpressionOutput Output;
	Output.OutputName = FName(TEXT("交NPC"));
	Output.OutputType = TOT_NPCExit_Test;
	Outputs.Add(Output);

	BeforePost.InputName = FName(TEXT("交之前"));
	BeforePost.InputType = TIT_EventInput;

	AfterPost.InputName = FName(TEXT("交之后"));
	AfterPost.InputType = TIT_EventInput;
}



UTaskSystemExpressionSelectSubtask::UTaskSystemExpressionSelectSubtask(
	FObjectInitializer const& Initializer)
	: Super(Initializer)
{
	
	BeforePost.InputName = FName(TEXT("选之前"));
	BeforePost.InputType = TIT_EventInput;

	AfterPost.InputName = FName(TEXT("选之后"));
	AfterPost.InputType = TIT_EventInput;
}