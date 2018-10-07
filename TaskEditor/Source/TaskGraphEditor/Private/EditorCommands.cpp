#include"EditorCommands.h"

// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.


#define LOCTEXT_NAMESPACE "TaskGraphEditor"

void FEditorCommands::RegisterCommands()
{
	UI_COMMAND(Export, "EditorToolBar", 
		"Execute EditorToolBar action",
		EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
