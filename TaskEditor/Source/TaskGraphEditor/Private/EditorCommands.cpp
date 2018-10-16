#include"EditorCommands.h"

// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.


#define LOCTEXT_NAMESPACE "TaskGraphEditor"

void FEditorCommands::RegisterCommands()
{
	UI_COMMAND(Export, "导出xlsx", 
		"导出xlsx",
		EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
