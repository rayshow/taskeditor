#pragma once

#include "CoreMinimal.h"
#include "Styling/CoreStyle.h"
#include "Framework/Commands/Commands.h"


class FEditorCommands 
	: public TCommands<FEditorCommands>
{
public:
	FEditorCommands()
		:TCommands<FEditorCommands>(TEXT("TaskGraphEditor"),
			NSLOCTEXT("TaskGraphEditor", "TaskSystem Editor", "Common Commands"),
			NAME_None, FCoreStyle::Get().GetStyleSetName())
	{}

	virtual ~FEditorCommands() {}

	virtual void RegisterCommands() override;

	TSharedPtr< FUICommandInfo > Export;

};