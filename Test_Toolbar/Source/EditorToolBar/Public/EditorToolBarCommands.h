// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "EditorToolBarStyle.h"

class FEditorToolBarCommands : public TCommands<FEditorToolBarCommands>
{
public:

	FEditorToolBarCommands()
		: TCommands<FEditorToolBarCommands>(TEXT("EditorToolBar"), NSLOCTEXT("Contexts", "EditorToolBar", "EditorToolBar Plugin"), NAME_None, FEditorToolBarStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
