// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "BentMaterialAutoGenCommands.h"

#define LOCTEXT_NAMESPACE "FBentMaterialAutoGenModule"

void FBentMaterialAutoGenCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "BentMaterialAutoGen", "Execute BentMaterialAutoGen action", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
