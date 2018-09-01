#pragma once

#include "Framework/Commands/Commands.h"
#include "EditorStyleSet.h"


#define LOCTEXT_NAMESPACE "DataTableEditor"

class DataTableConvertCommand
	: public TCommands<DataTableConvertCommand>
{
public:
	DataTableConvertCommand()
		: TCommands<DataTableConvertCommand>(
			TEXT("DataTableConvertCommand"),           // Context name for fast lookup
			NSLOCTEXT("DataTable2CSV", "DemoEditor", "Demo Editor"),
			NAME_None,                              // Parent
			FEditorStyle::GetStyleSetName()
			)
	{
	}

	// TCommand<> interface
	void RegisterCommands() override {

		UI_COMMAND(Convert2CSV, "Save CSV", "Performs a test command", EUserInterfaceActionType::Button, FInputGesture());
#undef LOCTEXT_NAMESPACE
	}
	// End of TCommand<> interface

public:
	TSharedPtr<FUICommandInfo> Convert2CSV;
};

#undef LOCTEXT_NAMESPACE