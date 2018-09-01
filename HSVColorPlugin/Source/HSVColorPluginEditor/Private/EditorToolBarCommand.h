#pragma once

#include "Commands.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "HSVColorPlugin"

class EditorToolBarCommand
	: public TCommands<EditorToolBarCommand>
{
public:
	EditorToolBarCommand()
		: TCommands<EditorToolBarCommand>(
			TEXT("DemoEditorExtensions"),           // Context name for fast lookup
			NSLOCTEXT("Contents", "DemoEditor", "Demo Editor"),
			NAME_None,                              // Parent
			FEditorStyle::GetStyleSetName()
			)
	{
	}

	// TCommand<> interface
	void RegisterCommands() override {

		UI_COMMAND(SaveCommand,     "Test Command", "Performs a test command", EUserInterfaceActionType::Button, FInputGesture());
		UI_COMMAND(AnalysisCommand, "Test Command", "Performs a test command", EUserInterfaceActionType::Button, FInputGesture());
		UI_COMMAND(ClearCommand,    "Test Command", "Performs a test command", EUserInterfaceActionType::Button, FInputGesture());
		UI_COMMAND(UseLeftCommand,  "Test Command", "Performs a test command", EUserInterfaceActionType::Button, FInputGesture());
		UI_COMMAND(UseRightCommand, "Test Command", "Performs a test command", EUserInterfaceActionType::Button, FInputGesture());
		UI_COMMAND(AnalysisFromFileCommand, "Test Command", "Performs a test command", EUserInterfaceActionType::Button, FInputGesture());
		

	}
	// End of TCommand<> interface

	TSharedPtr<FUICommandInfo> SaveCommand;
	TSharedPtr<FUICommandInfo> AnalysisCommand;
	TSharedPtr<FUICommandInfo> AnalysisFromFileCommand;
	TSharedPtr<FUICommandInfo> ClearCommand;
	TSharedPtr<FUICommandInfo> UseLeftCommand;
	TSharedPtr<FUICommandInfo> UseRightCommand;
};

#undef LOCTEXT_NAMESPACE