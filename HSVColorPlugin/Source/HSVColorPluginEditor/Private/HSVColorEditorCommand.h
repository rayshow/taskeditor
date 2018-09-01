#pragma once

#include "Commands.h"
#include "EditorStyleSet.h"

class FHSVColorEditorCommand 
	: public TCommands<FHSVColorEditorCommand>
{
public:
	FHSVColorEditorCommand()
		: TCommands<FHSVColorEditorCommand>(
			TEXT("DemoEditorExtensions"),           // Context name for fast lookup
			NSLOCTEXT("Contents", "DemoEditor", "Demo Editor"),
			NAME_None,                              // Parent
			FEditorStyle::GetStyleSetName()
			)
	{
	}

	// TCommand<> interface
	void RegisterCommands() override {
#define LOCTEXT_NAMESPACE "HSVColorPlugin"
		UI_COMMAND(TestCommand, "Test Command", "Performs a test command", EUserInterfaceActionType::Button, FInputGesture());
#undef LOCTEXT_NAMESPACE
	}
	// End of TCommand<> interface

public:
	TSharedPtr<FUICommandInfo> TestCommand;
};