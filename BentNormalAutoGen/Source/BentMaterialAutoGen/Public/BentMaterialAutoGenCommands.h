// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "BentMaterialAutoGenStyle.h"

class FBentMaterialAutoGenCommands : public TCommands<FBentMaterialAutoGenCommands>
{
public:

	FBentMaterialAutoGenCommands()
		: TCommands<FBentMaterialAutoGenCommands>(TEXT("BentMaterialAutoGen"), NSLOCTEXT("Contexts", "BentMaterialAutoGen", "BentMaterialAutoGen Plugin"), NAME_None, FBentMaterialAutoGenStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
