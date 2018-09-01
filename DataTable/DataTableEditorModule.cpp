// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "DataTableEditorModule.h"
#include "Modules/ModuleManager.h"
#include "IDataTableEditor.h"
#include "DataTableEditor.h"
#include "DataTableConvertCommands.h"

IMPLEMENT_MODULE(FDataTableEditorModule, DataTableEditor);


const FName FDataTableEditorModule::DataTableEditorAppIdentifier(TEXT("DataTableEditorApp"));

void FDataTableEditorModule::StartupModule()
{
	DataTableConvertCommand::Register();
	MenuExtensibilityManager = MakeShareable(new FExtensibilityManager);
}


void FDataTableEditorModule::ShutdownModule()
{
	DataTableConvertCommand::Unregister();
	MenuExtensibilityManager.Reset();
}


TSharedRef<IDataTableEditor> FDataTableEditorModule::CreateDataTableEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UDataTable* Table)
{
	TSharedRef< FDataTableEditor > NewDataTableEditor(new FDataTableEditor());
	NewDataTableEditor->InitDataTableEditor(Mode, InitToolkitHost, Table);
	return NewDataTableEditor;
}

