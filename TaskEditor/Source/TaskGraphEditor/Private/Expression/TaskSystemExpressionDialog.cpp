#include"Expression/TaskSystemExpressionDialog.h"

#pragma once

#include"Expression/TaskSystemExpressionSubtask.h"
#include"Expression/TaskSystemExpressionOutput.h"

UTaskSystemExpression1In2Dialog::UTaskSystemExpression1In2Dialog(
	FObjectInitializer const& Initializer) : Super(Initializer) {
	FTaskSystemExpressionOutput Output1;
	Output1.OutputName = FName(TEXT("是"));
	Output1.OutputType = TOT_BranchExit;
	FTaskSystemExpressionOutput Output2;
	Output2.OutputName = FName(TEXT("否"));
	Output2.OutputType = TOT_BranchExit;
	Outputs.Add(Output1);
	Outputs.Add(Output2);

	//Input
	Input.InputName = FName(TEXT("对话内容"));
	Input.InputType = TIT_DialogEntry;
}

