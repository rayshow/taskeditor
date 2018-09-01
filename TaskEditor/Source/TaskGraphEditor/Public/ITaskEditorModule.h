#pragma once


#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Framework/Commands/UICommandList.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "../Classes/TaskSystem.h"

class UTaskSystem;
class UTaskModule;
class ITaskEditor;

class ITaskEditorModule : public IModuleInterface,
	public IHasMenuExtensibility, public IHasToolBarExtensibility
{
public:

	//open from Task system
	virtual TSharedRef<ITaskEditor> CreateTaskEditor(const EToolkitMode::Type Mode,
		const TSharedPtr< IToolkitHost >& InitToolkitHost, UTaskSystem* TaskSystem) = 0;

	//open from Task Module
	virtual TSharedRef<ITaskEditor> CreateTaskEditor(const EToolkitMode::Type Mode,
		const TSharedPtr< IToolkitHost >& InitToolkitHost, UTaskModule* Module) = 0;


	/** Delegates to be called to extend the material menus */
	DECLARE_DELEGATE_RetVal_OneParam(TSharedRef<FExtender>, FTaskMenuExtender, const TSharedRef<FUICommandList>);
	DECLARE_DELEGATE_RetVal_OneParam(TSharedRef<FExtender>, FTaskMenuExtender_MaterialInterface, const UTaskSystem*);
	virtual TArray<FTaskMenuExtender>& GetAllTaskCanvasMenuExtenders() { return TaskCanvasMenuExtenders; }
	virtual TArray<FTaskMenuExtender_MaterialInterface>& GetAllTaskDragDropContextMenuExtenders() { return TaskInheritanceMenuExtenders; }


	/** Delegate to be called when a Material Editor is created, for toolbar, tab, and menu extension **/
	DECLARE_EVENT_OneParam(ITaskEditorModule, FTaskEditorOpenedEvent, TWeakPtr<ITaskEditor>);
	virtual FTaskEditorOpenedEvent& OnTaskEditorOpened() { return TaskEditorOpenedEvent; };


private:
	/** All extender delegates for the material menus */
	TArray<FTaskMenuExtender> TaskCanvasMenuExtenders;
	TArray<FTaskMenuExtender_MaterialInterface> TaskInheritanceMenuExtenders;

	FTaskEditorOpenedEvent TaskEditorOpenedEvent;
};