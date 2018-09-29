#pragma once

#include"Expression/TaskSystemExpressionSubtask.h"
#include"Expression/TaskSystemExpressionOutput.h"

UTaskSystemExpressionSubtask_Common::UTaskSystemExpressionSubtask_Common(
	FObjectInitializer const& Initializer)
	: Super(Initializer) {

	//Input
	Input.InputName = FName(TEXT("接NPC"));
	Input.InputType = TIT_NPCEntry_Test;
	Input.IsEvent = false;

	//Event
	PreAccept.InputName = FName(TEXT("接之前"));
	PreAccept.InputType = TIT_EventInput;
	PreAccept.IsEvent = true;

	PostAccept.InputName = FName(TEXT("接之后"));
	PostAccept.InputType = TIT_EventInput;
	PostAccept.IsEvent = true;
}


UTaskSystemExpressionSubtask::UTaskSystemExpressionSubtask(
	FObjectInitializer const& Initializer)
	: Super(Initializer) 
{
	FTaskSystemExpressionOutput Output;
	Output.OutputName = FName(TEXT("交NPC"));
	Output.OutputType = TOT_NPCExit_Test;
	Outputs.Add(Output);

	PreHandup.InputName = FName(TEXT("交之前"));
	PreHandup.InputType = TIT_EventInput;
	PreHandup.IsEvent = true;

	PostHandup.InputName = FName(TEXT("交之后"));
	PostHandup.InputType = TIT_EventInput;
	PostHandup.IsEvent = true;
}



UTaskSystemExpressionSelectSubtask::UTaskSystemExpressionSelectSubtask(
	FObjectInitializer const& Initializer)
	: Super(Initializer)
{
	
	PreSelect.InputName = FName(TEXT("选之前"));
	PreSelect.InputType = TIT_EventInput;
	PreSelect.IsEvent = true;

	PostSelect.InputName = FName(TEXT("选之后"));
	PostSelect.InputType = TIT_EventInput;
	PostSelect.IsEvent = true;

	FSubtargetSelectItem FirstSelect;
	FirstSelect.NextTaskID = 0;
	FirstSelect.DialogID = 0;
	FirstSelect.ItemDesc = FString(TEXT("是"));
	
	FSubtargetSelectItem SecondSelect;
	SecondSelect.NextTaskID = 0;
	SecondSelect.DialogID = 0;
	SecondSelect.ItemDesc = FString(TEXT("否"));

	Selects.Add(FirstSelect);
	Selects.Add(SecondSelect);

}