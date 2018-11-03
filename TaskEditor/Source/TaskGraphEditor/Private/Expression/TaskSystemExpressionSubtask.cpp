#pragma once

#include"Expression/TaskSystemExpressionSubtask.h"
#include"Expression/TaskSystemExpressionOutput.h"

static const FName NAME_Input_AcceptNPC(TEXT("接NPC"));
static const FName NAME_Event_BeforeAccept(TEXT("接之前"));
static const FName NAME_Event_PostAccept(TEXT("接之后"));



UTaskSystemExpressionSubtask_Common::UTaskSystemExpressionSubtask_Common(
	FObjectInitializer const& Initializer)
	: Super(Initializer)
{
	//Input
	Input.InputName = NAME_Input_AcceptNPC;
	Input.InputType = TIT_NPCEntry_Test;
	Input.IsEvent = false;
	
	//Event
	PreAccept.InputName = NAME_Event_BeforeAccept;
	PreAccept.InputType = TIT_EventInput;
	PreAccept.IsEvent = true;

	PostAccept.InputName = NAME_Event_PostAccept;
	PostAccept.InputType = TIT_EventInput;
	PostAccept.IsEvent = true;
}

void UTaskSystemExpressionSubtask_Common::NotifyLinkTo(FTaskSystemExpressionOutput* ThisOutput,
	UTaskSystemExpression* ToExpr, FTaskSystemExpressionInput* ToInput)
{
	
}

void UTaskSystemExpressionSubtask_Common::AddInputExpression(FTaskSystemExpressionInput* ThisInput,
	UTaskSystemExpression* FromExpr)
{
	
}


void UTaskSystemExpressionSubtask_Common::NotifyLinkFrom(FTaskSystemExpressionInput* ThisInput,
	UTaskSystemExpression* FromExpr, FTaskSystemExpressionOutput* FromOutput)
{
	//
	if (ThisInput == &Input)
	{
	/*	PreTaskID = FromExp->TaskID;
		ThisInput->PreTaskObjectIDs.Add(FromExp->TaskID);
		ThisInput->LinkedExprs.Add(FromExpr);*/
	}
}

static const FName NAME_Output_HandUpNpc(TEXT("交NPC"));
static const FName NAME_Event_BeforeHandUp(TEXT("交之前"));
static const FName NAME_Event_PostHandUp(TEXT("交之后"));

UTaskSystemExpressionSubtask::UTaskSystemExpressionSubtask(
	FObjectInitializer const& Initializer)
	: Super(Initializer) 
{
	FTaskSystemExpressionOutput Output;
	Output.OutputName = NAME_Output_HandUpNpc;
	Output.OutputType = TOT_NPCExit_Test;
	Outputs.Add(Output);

	PreHandup.InputName = NAME_Event_BeforeHandUp;
	PreHandup.InputType = TIT_EventInput;
	PreHandup.IsEvent = true;

	PostHandup.InputName = NAME_Event_PostHandUp;
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