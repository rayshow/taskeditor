
#include"Public/ITaskEditorModule.h"
#include"TaskSystem.h"
#include"TaskEditor.h"
#include"AssetToolsModule.h"
#include"EditorCommands.h"
#include"AssetTypeActions_TaskSystem.h"


class FTaskEditorModule : public ITaskEditorModule
{
public:
	
	virtual void StartupModule() override
	{
		MenuExtensibilityManager = MakeShareable(new FExtensibilityManager);
		ToolBarExtensibilityManager = MakeShareable(new FExtensibilityManager);
		FEditorCommands::Register();

		auto& AssetTool = FAssetToolsModule::GetModule().Get();
		AssetTool.RegisterAssetTypeActions(MakeShareable(new FAssetTypeActions_TaskSystem));
		AssetTool.RegisterAssetTypeActions(MakeShareable(new FAssetTypeActions_TaskModule));
	}

	
	virtual void ShutdownModule() override
	{
		FEditorCommands::Unregister();
		MenuExtensibilityManager.Reset();
		ToolBarExtensibilityManager.Reset();
	}

	
	virtual TSharedRef<ITaskEditor> CreateTaskEditor(const EToolkitMode::Type Mode, 
		const TSharedPtr< IToolkitHost >& InitToolkitHost, UTaskSystem* TaskSystem) override
	{
		TSharedRef<FTaskEditor> NewTaskEditor(new FTaskEditor());
		NewTaskEditor->InitEditorForTaskSystem(TaskSystem);
		NewTaskEditor->InitTaskEditor(Mode, InitToolkitHost, TaskSystem);
		return NewTaskEditor;
	}


	//open from Task Module
	virtual TSharedRef<ITaskEditor> CreateTaskEditor(const EToolkitMode::Type Mode,
		const TSharedPtr< IToolkitHost >& InitToolkitHost, UTaskModule* Module) override
	{
		TSharedRef<FTaskEditor> NewTaskEditor(new FTaskEditor());
		NewTaskEditor->InitEditorForTaskModule(Module);
		NewTaskEditor->InitTaskEditor(Mode, InitToolkitHost, Module);
		return NewTaskEditor;
	}


	/** Gets the extensibility managers for outside entities to extend material editor's menus and toolbars */
	virtual TSharedPtr<FExtensibilityManager> GetMenuExtensibilityManager() override { return MenuExtensibilityManager; }
	virtual TSharedPtr<FExtensibilityManager> GetToolBarExtensibilityManager() override { return ToolBarExtensibilityManager; }

private:
	TSharedPtr<FExtensibilityManager> MenuExtensibilityManager;
	TSharedPtr<FExtensibilityManager> ToolBarExtensibilityManager;

};

IMPLEMENT_MODULE(FTaskEditorModule, TaskGraphEditor);
