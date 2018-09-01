// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "EditorToolBarCommands.h"

#define LOCTEXT_NAMESPACE "FEditorToolBarModule"

void FEditorToolBarCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "EditorToolBar", "Execute EditorToolBar action", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
