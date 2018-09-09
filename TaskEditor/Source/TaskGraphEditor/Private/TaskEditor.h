#pragma once

#include"ITaskEditor.h"
#include"ScopedTransaction.h"
#include"GraphEditor.h"

class UTaskObject;
class UTaskSystem;
class UTaskModule;
class STaskSystemGraphEditor;
class UEdGraph;
struct FGraphActionMenuBuilder;

class FTaskEditor : 
	public ITaskEditor
{
public:
	FTaskEditor();

	// init Task Editor content 
	void InitEditorForTaskSystem(UTaskSystem* InTaskSystem);
	void InitEditorForTaskModule(UTaskModule* InTaskModule);
	void InitTaskEditor(const EToolkitMode::Type Mode, 
		const TSharedPtr< class IToolkitHost >& InitToolkitHost, UObject* ObjectToEdit);

	// override FAssetEditorToolkit method 
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;
	virtual FName GetToolkitFName() const override { return FName("TaskEditorToolkit"); };
	virtual FText GetBaseToolkitName() const override { return NSLOCTEXT("TaskEditor","TaskEditorToolkit", "TaskEditorToolkit"); };
	virtual FString GetWorldCentricTabPrefix() const override { return NSLOCTEXT("TaskEditor", "WorldCentricTabPrefix", "TaskGraph ").ToString(); }
	virtual FLinearColor GetWorldCentricTabColorScale() const override { return FLinearColor(1, 0, 0, 1); }


	// Gets the extensibility managers for outside entities to extend material editor's menus and toolbars 
	virtual TSharedPtr<FExtensibilityManager> GetMenuExtensibilityManager() override { return MenuExtensibilityManager; }
	virtual TSharedPtr<FExtensibilityManager> GetToolBarExtensibilityManager() override { return ToolBarExtensibilityManager; }


	// Create widget of this editor 
	void CreateInteralWidgets();
	TSharedRef<STaskSystemGraphEditor> CreateTaskGraphEditor(UEdGraph* Graph);


	// Task Graph Event 
	void OnSelectedNodesChanged(const FGraphPanelSelectionSet& InSet);
	void OnNodeDoubleClicked(class UEdGraphNode* Node);
	void OnNodeTitleCommitted(const FText& InText, ETextCommit::Type InCommittedType, UEdGraphNode* InNode);
	bool OnVerifyNodeTextCommit(const FText& InText, UEdGraphNode* InNode, FText& OutText);
	FReply OnSpawnGraphNodeByShortCut(FInputChord, const FVector2D&, UEdGraph* InGraph);

	//Callback
	void OnTaskTreeItemChanged(UTaskObject* InObject);

public:
	//Util Function
	static void GetTaskSystemExpressionActions(FGraphActionMenuBuilder& ActionMenuBuilder, 
		UObject* Object, FName Category);

	static void GetTaskSystemContextActions( FGraphActionMenuBuilder& ActionMenuBuilder,
		UObject* Object, TArray<FName> Categories);

	static int32 GetNumberOfSelectedNodes(const class UEdGraph* Graph) { return 0; }
	static UTaskSystemExpression* CreateNewTaskSystemExpression(UObject* ToFocus, UClass* NewExpressionClass,
		const FVector2D& NodePos, bool bAutoSelect, bool bAutoAssignResource);

private:

	//Dispatch Util Function
	UTaskSystemExpression * InternalCreateNewMaterialExpression(UClass* NewExpressionClass,
		const FVector2D& NodePos, bool bAutoSelect, bool bAutoAssignResource);

	// Menu / Toolbar manager exntender
	TSharedPtr<FExtensibilityManager> MenuExtensibilityManager;
	TSharedPtr<FExtensibilityManager> ToolBarExtensibilityManager;

	//Editing TaskObject
	UTaskObject* TaskObject;

	//Select TaskObject
	UTaskObject* SelectTaskObject;


	// widgets
	TSharedPtr<class IDetailsView> DetailsView;
	TSharedPtr<class STaskSystemGraphEditor> GraphEditor;
	TSharedPtr<class SMaterialEditor3DPreviewViewport> PreviewViewport;
	TSharedPtr<class STaskSystemPalette> Palette;
	TSharedPtr<class STaskTreeView> TaskTree;

	//Tab
	TSharedPtr<SDockTab> GraphCanvesTab;

	// for Undo/Redo
	FScopedTransaction* ScopedTransaction;


	//command list for this editor
	TSharedPtr<FUICommandList> GraphEditorCommands;

	//Tab Name
	const static FName PreviewTabName;
	const static FName DetailTabName;
	const static FName TaskGraphCanvasTabName;
	const static FName TaskTreeTabName;
	const static FName TaskFunctionPaletteTabName;

	//SpawnTab Callback
	TSharedRef<SDockTab> SpawnTab_Preview(const FSpawnTabArgs& InArgs);
	TSharedRef<SDockTab> SpawnTab_Detail(const FSpawnTabArgs& InArgs);
	TSharedRef<SDockTab> SpawnTab_Canvas(const FSpawnTabArgs& InArgs);
	TSharedRef<SDockTab> SpawnTab_TaskTree(const FSpawnTabArgs& InArgs);
	TSharedRef<SDockTab> SpawnTab_FunctionPalette(const FSpawnTabArgs& InArgs);

	//Generic Commands ops
	void SelectAllNodes();
	void DeleteSelectedNodes();
	void CopySelectedNodes();
	void PasteNodes();
	void CutSelectedNodes();
	void DuplicateNodes();
	//Generic Commands prediction
	bool CanSelectAllNodes() const;
	bool CanDeleteNodes() const;
	bool CanCopyNodes() const;
	bool CanPasteNodes() const;
	bool CanCutNodes() const;
	bool CanDuplicateNodes() const;

	//internal functions
	void DeleteNodes(const TArray<UEdGraphNode*>& NodesToDelete);
	bool CheckExpressionRemovalWarnings(const TArray<UEdGraphNode*>& NodesToRemove);
	void PasteNodesHere(const FVector2D& Location);
	virtual void UpdateMaterialAfterGraphChange();
	void DeleteSelectedDuplicatableNodes();
};

