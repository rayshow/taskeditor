#include "TaskEditor.h"
#include "EditorStyleSet.h"
#include "SDockTab.h"
#include "ModuleManager.h"
#include "EdGraph/EdGraph.h"
#include "GraphEditor.h"
#include "BlueprintEditorUtils.h"
#include "ToolkitManager.h"
#include "UnrealEd.h"
#include "GenericCommands.h"
#include "EdGraphUtilities.h"
#include "PlatformApplicationMisc.h"
#include "SNodePanel.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "EditorCommands.h"

#include "ITaskEditorModule.h"
#include "TaskSystem.h"
#include "TaskSystemGraphNode.h"
#include "TaskSystemGraph.h"
#include "TaskSystemGraphSchema.h"
#include "STaskSystemGraphEditor.h"
#include "STaskSystemPalette.h"
#include "EditorCommands.h"
#include "STaskTreeView.h"
#include "Expression/ExpressionIncludes.h"
#include "Exports/XlsxExporter.h"

#include "TaskNodeFactory.h"

#define LOCTEXT_NAMESPACE "TaskGraphEditor"
const FName FTaskEditor::PreviewTabName(TEXT("TaskEditor_Preview"));
const FName FTaskEditor::DetailTabName(TEXT("TaskEditor_Detail"));
const FName FTaskEditor::TaskGraphCanvasTabName(TEXT("TaskEditor_Canves"));;
const FName FTaskEditor::TaskTreeTabName(TEXT("TaskEditor_TaskTree"));;
const FName FTaskEditor::TaskFunctionPaletteTabName(TEXT("TaskEditor_Palette"));;
const FName TaskEditorAppIdentifier = FName(TEXT("TaskEditorApp"));

FTaskEditor::FTaskEditor() :
	MenuExtensibilityManager(new FExtensibilityManager()),
	ToolBarExtensibilityManager(new FExtensibilityManager())
{
	/*FString Path = FPaths::GameContentDir() + L"example_.xlsx";
	Excel Exl(Path);
	if (Exl)
	{
		int b = 0;
	}
	int  a = 0;*/
}

void FTaskEditor::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_MaterialEditor", "Task Graph Editor"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(PreviewTabName, FOnSpawnTab::CreateSP(this, &FTaskEditor::SpawnTab_Preview))
		.SetDisplayName(LOCTEXT("ViewportTab", "Viewport"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Viewports"));


	InTabManager->RegisterTabSpawner(DetailTabName, FOnSpawnTab::CreateSP(this, &FTaskEditor::SpawnTab_Detail))
		.SetDisplayName(LOCTEXT("DetailTab", "Detail"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));


	InTabManager->RegisterTabSpawner(TaskGraphCanvasTabName, FOnSpawnTab::CreateSP(this, &FTaskEditor::SpawnTab_Canvas))
		.SetDisplayName(LOCTEXT("CanvasTab", "Canvas"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "GraphEditor.EventGraph_16x"));
	
	InTabManager->RegisterTabSpawner(TaskTreeTabName, FOnSpawnTab::CreateSP(this, &FTaskEditor::SpawnTab_TaskTree))
		.SetDisplayName(LOCTEXT("TaskTreeTab", "TaskTree"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Viewports"));

	InTabManager->RegisterTabSpawner(TaskFunctionPaletteTabName, FOnSpawnTab::CreateSP(this, &FTaskEditor::SpawnTab_FunctionPalette))
		.SetDisplayName(LOCTEXT("PaletteTab", "Palette"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "Kismet.Tabs.Palette"));


}

void FTaskEditor::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

	InTabManager->UnregisterTabSpawner(PreviewTabName);
	InTabManager->UnregisterTabSpawner(DetailTabName);
	InTabManager->UnregisterTabSpawner(TaskGraphCanvasTabName);
	InTabManager->UnregisterTabSpawner(TaskTreeTabName);
	InTabManager->UnregisterTabSpawner(TaskFunctionPaletteTabName);
}

TSharedRef<SDockTab> FTaskEditor::SpawnTab_Preview(const FSpawnTabArgs& InArgs)
{
	check(InArgs.GetTabId() == PreviewTabName);

	TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
		.Label(LOCTEXT("ViewportTabTitle", "Preview"))
		[
			SNew(SOverlay) 
		];
 
	return SpawnedTab;
}

TSharedRef<SDockTab> FTaskEditor::SpawnTab_Detail(const FSpawnTabArgs& InArgs)
{
	if(Cast<UTaskSystem>(TaskObject))
		DetailsView->SetObject(Cast<UTaskSystem>(TaskObject));
	else if(Cast<UTaskModule>(TaskObject))
		DetailsView->SetObject(Cast<UTaskModule>(TaskObject));
	TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
		.Label(LOCTEXT("DetailPanel", "DetailPanel"))
		[
			DetailsView.ToSharedRef()
		];

	return SpawnedTab;
}

TSharedRef<SDockTab> FTaskEditor::SpawnTab_Canvas(const FSpawnTabArgs& InArgs)
{
	GraphCanvesTab = SNew(SDockTab)
		.Label(LOCTEXT("TaskCanvasTitle", "TaskGraph"));

	if (GraphEditor.IsValid())
	{
		GraphCanvesTab->SetContent(GraphEditor.ToSharedRef());
	}

	return GraphCanvesTab.ToSharedRef();
}

TSharedRef<SDockTab> FTaskEditor::SpawnTab_TaskTree(const FSpawnTabArgs& InArgs)
{
	TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
		.Label(LOCTEXT("TaskTreeTitle", "TaskHierarchy"))
		[
			TaskTree.ToSharedRef()
		];

	return SpawnedTab;
}

TSharedRef<SDockTab> FTaskEditor::SpawnTab_FunctionPalette(const FSpawnTabArgs& InArgs)
{
	TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
		.Label(LOCTEXT("FunctionTabTitle", "Functions"))
		[
			Palette.ToSharedRef()
		];

	return SpawnedTab;
}


void FTaskEditor::InitEditorForTaskSystem(UTaskSystem* InTaskSystem)
{
	check(InTaskSystem);

	TaskObject = InTaskSystem;
	SelectTaskObject = InTaskSystem;
	TaskObject->Initialize();
}

void FTaskEditor::InitEditorForTaskModule(UTaskModule* InTaskModule)
{
	check(InTaskModule);
	TaskObject = InTaskModule;
	SelectTaskObject = InTaskModule;
	TaskObject->Initialize();

}


void FTaskEditor::InitTaskEditor(const EToolkitMode::Type Mode,
	const TSharedPtr< class IToolkitHost >& InitToolkitHost, UObject* ObjectToEdit)
{
	TaskObject->SetFlags(RF_Transactional);

	
	CreateInteralWidgets();

	const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("Standalone_TaskEditor_Layout_v6")
		->AddArea
		(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.1f)
				->SetHideTabWell(true)
				->AddTab(GetToolbarTabId(), ETabState::OpenedTab)
			)
			->Split
			(
				FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)->SetSizeCoefficient(0.9f)
				->Split
				(
					FTabManager::NewSplitter()->SetOrientation(Orient_Vertical)->SetSizeCoefficient(0.2f)
					->Split
					(
						FTabManager::NewStack()
						//->SetHideTabWell(true)
						->SetSizeCoefficient(0.4f)
						->AddTab(PreviewTabName, ETabState::OpenedTab)
					)
					->Split
					(
						FTabManager::NewStack()
						->AddTab(DetailTabName, ETabState::OpenedTab)
					)
				)
				->Split
				(
					FTabManager::NewSplitter()->SetOrientation(Orient_Vertical)
					->SetSizeCoefficient(0.80f)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.8f)
						//->SetHideTabWell(true)
						->AddTab(TaskGraphCanvasTabName, ETabState::OpenedTab)
					)
				)
				->Split
				(
					FTabManager::NewSplitter()->SetOrientation(Orient_Vertical)->SetSizeCoefficient(0.2f)
					->Split
					(
						FTabManager::NewSplitter()->SetOrientation(Orient_Vertical)
						->Split
						(
							FTabManager::NewStack()
							->SetSizeCoefficient(0.4f)
							//->SetHideTabWell(true)
							->AddTab(TaskTreeTabName, ETabState::OpenedTab)
						)
						->Split
						(
							FTabManager::NewStack()
							->SetSizeCoefficient(0.6f)
							->AddTab(TaskFunctionPaletteTabName, ETabState::OpenedTab)
						)
					)
				)
			)
		);

	// Generate Window
	TArray< UObject* > ObjectsToEdit;
	ObjectsToEdit.Add(TaskObject);
	bool bCreateDefaultStandaloneMenu = true;
	bool bCreateDefaultToolbar = true;

	FAssetEditorToolkit::InitAssetEditor(Mode, InitToolkitHost, TaskEditorAppIdentifier,
		StandaloneDefaultLayout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, ObjectsToEdit, false);

	//
	ToolkitCommands->MapAction(
		FEditorCommands::Get().Export,
		FExecuteAction::CreateSP(this, &FTaskEditor::ExportTasks),
		FCanExecuteAction());

	struct Local
	{
		static void FillToolbar(FToolBarBuilder& ToolbarBuilder)
		{
			ToolbarBuilder.BeginSection("Export");
			{
				ToolbarBuilder.AddToolBarButton(FEditorCommands::Get().Export,
					NAME_None);
			}
			ToolbarBuilder.EndSection();

		}
	};

	TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);

	ToolbarExtender->AddToolBarExtension(
		"Asset",
		EExtensionHook::After,
		GetToolkitCommands(),
		FToolBarExtensionDelegate::CreateStatic(&Local::FillToolbar)
	);

	AddToolbarExtender(ToolbarExtender);




	// add menu 
	AddMenuExtender(GetMenuExtensibilityManager()->GetAllExtenders(GetToolkitCommands(), GetEditingObjects()));
	ITaskEditorModule* TaskEditorModule = &FModuleManager::LoadModuleChecked<ITaskEditorModule>("TaskGraphEditor");
	AddMenuExtender(TaskEditorModule->GetMenuExtensibilityManager()->GetAllExtenders(GetToolkitCommands(), GetEditingObjects()));

	// add toolbar
	AddToolbarExtender(GetToolBarExtensibilityManager()->GetAllExtenders(GetToolkitCommands(), GetEditingObjects()));
	AddToolbarExtender(TaskEditorModule->GetToolBarExtensibilityManager()->GetAllExtenders(GetToolkitCommands(), GetEditingObjects()));

	RegenerateMenusAndToolbars();
}



TSharedRef<STaskSystemGraphEditor> FTaskEditor::CreateTaskGraphEditor(UEdGraph* Graph)
{
	GraphEditorCommands = MakeShareable(new FUICommandList);
	{
		// Editing commands
		GraphEditorCommands->MapAction(FGenericCommands::Get().SelectAll,
			FExecuteAction::CreateSP(this, &FTaskEditor::SelectAllNodes),
			FCanExecuteAction::CreateSP(this, &FTaskEditor::CanSelectAllNodes)
		);

		GraphEditorCommands->MapAction(FGenericCommands::Get().Delete,
			FExecuteAction::CreateSP(this, &FTaskEditor::DeleteSelectedNodes),
			FCanExecuteAction::CreateSP(this, &FTaskEditor::CanDeleteNodes)
		);

		GraphEditorCommands->MapAction(FGenericCommands::Get().Copy,
			FExecuteAction::CreateSP(this, &FTaskEditor::CopySelectedNodes),
			FCanExecuteAction::CreateSP(this, &FTaskEditor::CanCopyNodes)
		);

		GraphEditorCommands->MapAction(FGenericCommands::Get().Paste,
			FExecuteAction::CreateSP(this, &FTaskEditor::PasteNodes),
			FCanExecuteAction::CreateSP(this, &FTaskEditor::CanPasteNodes)
		);

		GraphEditorCommands->MapAction(FGenericCommands::Get().Cut,
			FExecuteAction::CreateSP(this, &FTaskEditor::CutSelectedNodes),
			FCanExecuteAction::CreateSP(this, &FTaskEditor::CanCutNodes)
		);

		GraphEditorCommands->MapAction(FGenericCommands::Get().Duplicate,
			FExecuteAction::CreateSP(this, &FTaskEditor::DuplicateNodes),
			FCanExecuteAction::CreateSP(this, &FTaskEditor::CanDuplicateNodes)
		);
	}


	FGraphAppearanceInfo AppearanceInfo;
	AppearanceInfo.CornerText = LOCTEXT("AppearanceCornerText_TaskSystem", "TaskSystem");


	SGraphEditor::FGraphEditorEvents InEvents;
	InEvents.OnSelectionChanged.BindRaw(this, &FTaskEditor::OnSelectedNodesChanged);
	InEvents.OnNodeDoubleClicked = FSingleNodeEvent::CreateRaw(this, &FTaskEditor::OnNodeDoubleClicked);
	InEvents.OnTextCommitted = FOnNodeTextCommitted::CreateRaw(this, &FTaskEditor::OnNodeTitleCommitted);
	InEvents.OnVerifyTextCommit = FOnNodeVerifyTextCommit::CreateRaw(this, &FTaskEditor::OnVerifyNodeTextCommit);
	InEvents.OnSpawnNodeByShortcut = SGraphEditor::FOnSpawnNodeByShortcut::CreateRaw(this,
		&FTaskEditor::OnSpawnGraphNodeByShortCut, Cast<UEdGraph>(Graph));

	// Create the title bar widget
	/*SharedPtr<SWidget> TitleBarWidget = SNew(SMaterialEditorTitleBar)
		.TitleText(this, &FMaterialEditor::GetOriginalObjectName);*/
	//.MaterialInfoList(&MaterialInfoList);

	auto Editor = SNew(STaskSystemGraphEditor)
		.AdditionalCommands(GraphEditorCommands)
		.IsEditable(true)
		//.TitleBar(TitleBarWidget)
		.Appearance(AppearanceInfo)
		.GraphToEdit(Graph)
		.GraphEvents(InEvents)
		.ShowGraphStateOverlay(false);
	Editor->SetNodeFactory(MakeShareable(new FTaskNodeGraphFactory));
	return Editor;
}

void FTaskEditor::CreateInteralWidgets()
{
	//Create Graph Editor 
	GraphEditor = CreateTaskGraphEditor(TaskObject->Graph);
	GraphEditor->SetViewLocation(FVector2D::ZeroVector, 1);


	//Create Palette
	Palette = SNew(STaskSystemPalette, SharedThis(this))
		.OnTaskObjectChanged_Lambda([this]() {
		return SelectTaskObject;
	});

	//Task Tree
	TaskTree = SNew(STaskTreeView)
		.OnSingleItemSelected(this, &FTaskEditor::OnTaskTreeItemChanged);

	check(TaskTree.IsValid());
	if (TaskObject->GetClass()->IsChildOf<UTaskModule>())
	{
		TaskTree->AddTaskModule(Cast<UTaskModule>(TaskObject));
	}
	else if(TaskObject->GetClass()->IsChildOf<UTaskSystem>()){
		UTaskSystem* System = Cast<UTaskSystem>(TaskObject);
		for (auto Module : System->Modules) {
			TaskTree->AddTaskModule(Cast<UTaskModule>(Module));
		}
	}

	//Detail Panel
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs(false, false, false, FDetailsViewArgs::HideNameArea,false, this);
	DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	DetailsView->SetIsPropertyVisibleDelegate(FIsPropertyVisible::CreateLambda([](const FPropertyAndParent&) {return true; }));
}


void FTaskEditor::NotifyPostChange(
	const FPropertyChangedEvent& PropertyChangedEvent,
	UProperty* PropertyThatChanged) {
	int Num = PropertyChangedEvent.GetNumObjectsBeingEdited();
	for (int i = 0; i < Num; ++i) {
		auto ObjectBeingEdit = PropertyChangedEvent.GetObjectBeingEdited(i);
		auto Expr = Cast<UTaskSystemExpression>(ObjectBeingEdit);
		if (Expr) {
			FString ChangePropertyName = PropertyChangedEvent.MemberProperty->GetName();
			if (Expr->OnExpressionChanged.IsBound())
				Expr->OnExpressionChanged.Execute(ChangePropertyName);
		}
	}
}

void FTaskEditor::OnSelectedNodesChanged(const FGraphPanelSelectionSet& InSet) {
	if (InSet.Num() == 1)
	{
		UTaskSystemGraphNode* Node = Cast<UTaskSystemGraphNode>(InSet.Array()[0]);
		check(Node);

		auto Expr = Cast<UTaskSystemExpression>(Node->Expression);
		check(Expr);
		if (auto Expr2 = Cast<UTaskSystemExpression1In2Dialog>(Expr)) {
			DetailsView->SetObject(Expr2);
		}
		else if (auto Expr2 = Cast<UTaskSystemExpressionSpawnMonster>(Expr)) {
			DetailsView->SetObject(Expr2);
		}
		else if (auto Expr2 = Cast<UTaskSystemExpressionSubtask>(Expr)) {
			DetailsView->SetObject(Expr2);
		}
		else if (auto Expr2 = Cast<UTaskSystemExpressionWeather>(Expr)) {
			DetailsView->SetObject(Expr2);
		}
		else if (auto Expr2 = Cast<UTaskSystemExpressionSelectSubtask>(Expr)) {
			DetailsView->SetObject(Expr2);
		}
		else {
			check(false);
		}
		
	}
}

void FTaskEditor::OnNodeDoubleClicked(class UEdGraphNode* Node) {

}

void FTaskEditor::OnNodeTitleCommitted(const FText& InText, ETextCommit::Type InCommittedType, UEdGraphNode* InNode) {

}

bool FTaskEditor::OnVerifyNodeTextCommit(const FText& InText, UEdGraphNode* InNode, FText& OutText) {
	return true;
}

FReply FTaskEditor::OnSpawnGraphNodeByShortCut(FInputChord, const FVector2D&, UEdGraph* InGraph) {
	return FReply::Handled();
}

void FTaskEditor::OnTaskTreeItemChanged(UTaskObject* InObject)
{
	if (InObject) {
		DetailsView->SetObject(InObject);
		GraphEditor = CreateTaskGraphEditor(InObject->Graph);
		GraphEditor->SetViewLocation(FVector2D::ZeroVector, 1);
		GraphCanvesTab->SetContent(GraphEditor.ToSharedRef());

		SelectTaskObject = InObject;
		Palette->NotifyTaskObjectChanged();
	}
	//else {
	//	UTaskSystem* System = Cast<UTaskSystem>(TaskObject);
	//	UTaskModule* Module = Cast<UTaskModule>(TaskObject);
	//	if (System) {
	//		DetailsView->SetObject(System);
	//	}
	//	else if(Module)
	//	{
	//		DetailsView->SetObject(Module);
	//	}
	//	else {
	//		check(false);
	//	}
	//	SelectTaskObject = TaskObject;
	//}
}

void FTaskEditor::GetTaskSystemExpressionActions(
	FGraphActionMenuBuilder& ActionMenuBuilder, UObject* Object, FName Category)
{
	if (Object && Object->GetClass()->IsChildOf<UTask>())
	{
		TaskSystemExpressionClasses* Classes = TaskSystemExpressionClasses::Get();
		Classes->AddExpressions(ActionMenuBuilder, FText::FromString(Category.ToString()), &Classes->AllExpressionClasses);
	}
}

void FTaskEditor::GetTaskSystemContextActions(
	FGraphActionMenuBuilder& ActionMenuBuilder, UObject* Object, TArray<FName> Categories)
{
	if (Object && Object->GetClass()->IsChildOf<UTask>())
	{
		TaskSystemExpressionClasses* Classes = TaskSystemExpressionClasses::Get();
		if (Categories.Contains(UTaskSystemGraphSchema::PC_All))
		{
			Classes->AddExpressions(ActionMenuBuilder,
				FText::FromString(UTaskSystemGraphSchema::PC_All.ToString()), &Classes->AllExpressionClasses);
		}
		else {
			if (Categories.Contains(UTaskSystemGraphSchema::PC_Event))
			{
				Classes->AddExpressions(ActionMenuBuilder,
					FText::FromString(UTaskSystemGraphSchema::PC_Event.ToString()), &Classes->EventExpressionClasses);
			}
			if (Categories.Contains(UTaskSystemGraphSchema::PC_Flow))
			{
				Classes->AddExpressions(ActionMenuBuilder,
					FText::FromString(UTaskSystemGraphSchema::PC_Flow.ToString()), &Classes->FlowExpressionClasses);
			}
			if (Categories.Contains(UTaskSystemGraphSchema::PC_TaskSubtarget))
			{
				Classes->AddExpressions(ActionMenuBuilder,
					FText::FromString(UTaskSystemGraphSchema::PC_TaskSubtarget.ToString()), &Classes->SubtargetExpressionClasses);
			}
		}
	}
}


UTaskSystemExpression* FTaskEditor::CreateNewTaskSystemExpression(
	UObject* ToFocus, UClass* NewExpressionClass, const FVector2D& NodePos, bool bAutoSelect, bool bAutoAssignResource)
{
	// Find the associated Material
	TSharedPtr<FTaskEditor> TaskEditor;

	UTaskSystem* TaskSystem = nullptr;
	UTaskModule* TaskModule = nullptr;

	UObject* Outer = ToFocus->GetOuter();
	while (!Outer->GetOuter()->GetClass()->IsChildOf<UPackage>())
	{
		Outer = Outer->GetOuter();
	}
	
	if ((TaskSystem = Cast<UTaskSystem>(Outer))!=nullptr)
	{
		TSharedPtr< IToolkit > FoundAssetEditor = FToolkitManager::Get().FindEditorForAsset(TaskSystem);
		if (FoundAssetEditor.IsValid())
		{
			TaskEditor = StaticCastSharedPtr<FTaskEditor>(FoundAssetEditor);
		}
		else {
			check(false);
		}
	}
	else if((TaskModule = Cast<UTaskModule>(Outer) ) != nullptr) {
		TSharedPtr< IToolkit > FoundAssetEditor = FToolkitManager::Get().FindEditorForAsset(TaskModule);
		if (FoundAssetEditor.IsValid())
		{
			TaskEditor = StaticCastSharedPtr<FTaskEditor>(FoundAssetEditor);
		}
		else{ check(false); }
	}
	else {
		check(false);
	}
	return TaskEditor->InternalCreateNewMaterialExpression(NewExpressionClass, NodePos, bAutoSelect, bAutoAssignResource);
}


UTaskSystemExpression* FTaskEditor::InternalCreateNewMaterialExpression(UClass* NewExpressionClass,
	const FVector2D& NodePos, bool bAutoSelect, bool bAutoAssignResource)
{
	check(NewExpressionClass->IsChildOf(UTaskSystemExpression::StaticClass()));


	// Clear the selection
	if (bAutoSelect)
	{
		GraphEditor->ClearSelectionSet();
	}

	// Create the new expression.
	UTaskSystemExpression* NewExpression = NULL;
	{
		const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "MaterialEditorNewExpression", "Material Editor: New Expression"));
		SelectTaskObject->Modify();

		UObject* SelectedAsset = nullptr;
		if (bAutoAssignResource)
		{
			SelectedAsset = GEditor->GetSelectedObjects()->GetTop<UObject>();
		}

		NewExpression = NewObject<UTaskSystemExpression>(SelectTaskObject, NewExpressionClass, NAME_None, RF_Transactional);
		check(NewExpression);
		if (NewExpression)
		{
			//TaskSystem->Expressions.Add(NewExpression);
			SelectTaskObject->Expressions.Add(NewExpression);
			NewExpression->TaskObject = SelectTaskObject;
		}
		NewExpression->MarkPackageDirty();

		if (NewExpression)
		{
			SelectTaskObject->Graph->AddExpression(NewExpression, bAutoSelect);
			NewExpression->GraphNode->NodePosX = NodePos.X;
			NewExpression->GraphNode->NodePosY = NodePos.Y;

			// Select the new node.
			if (bAutoSelect)
			{
				GraphEditor->SetNodeSelection(NewExpression->GraphNode, true);
			}
		}
	}

	SelectTaskObject->MarkPackageDirty();
	GraphEditor->NotifyGraphChanged();
	return NewExpression;
}



void FTaskEditor::SelectAllNodes()
{
	GraphEditor->SelectAllNodes();
}

void FTaskEditor::DeleteSelectedNodes() {
	TArray<UEdGraphNode*> NodesToDelete;
	const FGraphPanelSelectionSet SelectedNodes = GraphEditor->GetSelectedNodes();

	for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
	{
		NodesToDelete.Add(CastChecked<UEdGraphNode>(*NodeIt));
	}

	DeleteNodes(NodesToDelete);
}

void FTaskEditor::CopySelectedNodes() {
	// Export the selected nodes and place the text on the clipboard
	const FGraphPanelSelectionSet SelectedNodes = GraphEditor->GetSelectedNodes();

	FString ExportedText;

	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		if (UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter))
		{
			Node->PrepareForCopying();
		}
	}

	FEdGraphUtilities::ExportNodesToText(SelectedNodes, /*out*/ ExportedText);
	FPlatformApplicationMisc::ClipboardCopy(*ExportedText);

	// Make sure Material remains the owner of the copied nodes
	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		if (UMaterialGraphNode* Node = Cast<UMaterialGraphNode>(*SelectedIter))
		{
			Node->PostCopyNode();
		}
		/*else if (UMaterialGraphNode_Comment* Comment = Cast<UMaterialGraphNode_Comment>(*SelectedIter))
		{
			Comment->PostCopyNode();
		}*/
	}
}

void FTaskEditor::PasteNodes() {
	PasteNodesHere(GraphEditor->GetPasteLocation());
}

void FTaskEditor::CutSelectedNodes() {
	CopySelectedNodes();
	// Cut should only delete nodes that can be duplicated
	DeleteSelectedDuplicatableNodes();
}

void FTaskEditor::DuplicateNodes() {
	CopySelectedNodes();
	PasteNodes();
}



static void CollectExpressions(
	const FString& ModuleName,
	UTaskGroup* Module, ExportItem& Pair)
{
	for (auto Child : Module->Children) {
		if (auto Group = Cast<UTaskGroup>(Child))
		{
			CollectExpressions(ModuleName, Group, Pair);
		}
		else if (auto Task = Cast<UTask>(Child))
		{
			for (auto Expr : Task->Expressions)
			{
				if (auto SubTask = Cast<UTaskSystemExpressionSubtask_Common>(Expr))
				{
					Pair.Exprs.Add(SubTask);
				}
			}
				
		}
		else {
			check(false);
		}
	}
}

void FTaskEditor::ExportTasks() {
	TArray<ExportItem> AllPair;

	if (auto Task = Cast<UTaskSystem>(TaskObject)) {
		for (auto Child : Task->Modules)
		{
			ExportItem ModulePair;
			ModulePair.ModuleName = Child->GetPathName();
			CollectExpressions(Child->GetPathName(), Child, ModulePair);
			AllPair.Add(ModulePair);
		}
	}
	else if (auto Module = Cast<UTaskModule>(TaskObject)) {
		ExportItem ModulePair;
		ModulePair.ModuleName = Module->GetPathName();
		CollectExpressions(Module->GetPathName(), Module, ModulePair);
		AllPair.Add(ModulePair);
	}
	else {
		check(false);
	}
	
	for (auto& Item : AllPair)
	{
		XlsxExporter::ExportModule(Item);
	}
}


bool FTaskEditor::CanSelectAllNodes() const {
	return GraphEditor.IsValid();
}

bool FTaskEditor::CanDeleteNodes() const {
	const FGraphPanelSelectionSet SelectedNodes = GraphEditor->GetSelectedNodes();
	bool bDeletableNodeExists = false;

	for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
	{
		UEdGraphNode* GraphNode = Cast<UEdGraphNode>(*NodeIt);
		if (GraphNode && GraphNode->CanUserDeleteNode())
		{
			bDeletableNodeExists = true;
		}
	}

	return SelectedNodes.Num() > 0 && bDeletableNodeExists;
}

bool FTaskEditor::CanCopyNodes() const {
	const FGraphPanelSelectionSet SelectedNodes = GraphEditor->GetSelectedNodes();
	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if ((Node != NULL) && Node->CanDuplicateNode())
		{
			return true;
		}
	}
	return false;
}

bool FTaskEditor::CanPasteNodes() const
{
	FString ClipboardContent;
	FPlatformApplicationMisc::ClipboardPaste(ClipboardContent);
	return FEdGraphUtilities::CanImportNodesFromText(TaskObject->Graph, ClipboardContent);
}

bool FTaskEditor::CanCutNodes() const {
	return CanCopyNodes() && CanDeleteNodes();
}

bool FTaskEditor::CanDuplicateNodes() const {
	return CanCopyNodes();
}


bool FTaskEditor::CheckExpressionRemovalWarnings(const TArray<UEdGraphNode*>& NodesToRemove)
{
	return true;
}

void FTaskEditor::DeleteNodes(const TArray<UEdGraphNode*>& NodesToDelete)
{
	if (NodesToDelete.Num() > 0)
	{
		if (!CheckExpressionRemovalWarnings(NodesToDelete))
		{
			return;
		}

		// If we are previewing an expression and the expression being previewed was deleted
		bool bHaveExpressionsToDelete = false;
		bool bPreviewExpressionDeleted = false;

		{
			const FScopedTransaction Transaction(LOCTEXT("MaterialEditorDelete", "Material Editor: Delete"));
			TaskObject->Modify();

			for (int32 Index = 0; Index < NodesToDelete.Num(); ++Index)
			{
				if (NodesToDelete[Index]->CanUserDeleteNode())
				{
					// Break all node links first so that we don't update the material before deleting
					NodesToDelete[Index]->BreakAllNodeLinks();

					FBlueprintEditorUtils::RemoveNode(NULL, NodesToDelete[Index], true);

					if (UTaskSystemGraphNode* GraphNode = Cast<UTaskSystemGraphNode>(NodesToDelete[Index]))
					{
						UTaskSystemExpression* Expression = GraphNode->Expression;

						bHaveExpressionsToDelete = true;
						Expression->Modify();
						if (Expression->TaskObject && Expression->TaskObject->Expressions.Find(Expression) != INDEX_NONE) {
							Expression->TaskObject->Expressions.Remove(Expression);
							Expression->TaskObject->Modify();
							Expression->TaskObject->MarkPackageDirty();
						}
							
						//TaskSystem->Expressions.Remove(Expression);
						//TaskSystem->RemoveExpressionParameter(Expression);
						Expression->MarkPendingKill();
					}
				}
			}

			TaskObject->Graph->LinkTaskSystemExpressionsFromGraph();
		} // ScopedTransaction

		  // Deselect all expressions and comments.
		GraphEditor->ClearSelectionSet();
		GraphEditor->NotifyGraphChanged();
		TaskObject->MarkPackageDirty();
	}
}


void FTaskEditor::PasteNodesHere(const FVector2D& Location)
{
	// Undo/Redo support
	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "MaterialEditorPaste", "Material Editor: Paste"));
	TaskObject->Graph->Modify();
	TaskObject->Modify();

	// Clear the selection set (newly pasted stuff will be selected)
	GraphEditor->ClearSelectionSet();

	// Grab the text to paste from the clipboard.
	FString TextToImport;
	FPlatformApplicationMisc::ClipboardPaste(TextToImport);

	// Import the nodes
	TSet<UEdGraphNode*> PastedNodes;
	FEdGraphUtilities::ImportNodesFromText(TaskObject->Graph, TextToImport, /*out*/ PastedNodes);

	//Average position of nodes so we can move them while still maintaining relative distances to each other
	FVector2D AvgNodePosition(0.0f, 0.0f);

	for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
	{
		UEdGraphNode* Node = *It;
		AvgNodePosition.X += Node->NodePosX;
		AvgNodePosition.Y += Node->NodePosY;
	}

	if (PastedNodes.Num() > 0)
	{
		float InvNumNodes = 1.0f / float(PastedNodes.Num());
		AvgNodePosition.X *= InvNumNodes;
		AvgNodePosition.Y *= InvNumNodes;
	}

	for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
	{
		UEdGraphNode* Node = *It;
		if (UTaskSystemGraphNode* GraphNode = Cast<UTaskSystemGraphNode>(Node))
		{
			// These are not copied and we must account for expressions pasted between different materials anyway
			/*GraphNode->RealtimeDelegate = TaskSystem->Graph->RealtimeDelegate;
			GraphNode->MaterialDirtyDelegate = Material->MaterialGraph->MaterialDirtyDelegate;
			GraphNode->bPreviewNeedsUpdate = false; */

			UTaskSystemExpression* NewExpression = GraphNode->Expression;
			NewExpression->TaskObject = TaskObject;
			//TaskSystem->Expressions.Add(NewExpression);
		}
		/*else if (UMaterialGraphNode_Comment* CommentNode = Cast<UMaterialGraphNode_Comment>(Node))
		{
			if (CommentNode->MaterialExpressionComment)
			{
				CommentNode->MaterialDirtyDelegate = Material->MaterialGraph->MaterialDirtyDelegate;
				CommentNode->MaterialExpressionComment->Material = Material;
				CommentNode->MaterialExpressionComment->Function = MaterialFunction;
				Material->EditorComments.Add(CommentNode->MaterialExpressionComment);
			}
		}*/

		// Select the newly pasted stuff
		GraphEditor->SetNodeSelection(Node, true);

		Node->NodePosX = (Node->NodePosX - AvgNodePosition.X) + Location.X;
		Node->NodePosY = (Node->NodePosY - AvgNodePosition.Y) + Location.Y;

		Node->SnapToGrid(SNodePanel::GetSnapGridSize());

		// Give new node a different Guid from the old one
		Node->CreateNewGuid();
	}

	UpdateMaterialAfterGraphChange();

	// Update UI
	GraphEditor->NotifyGraphChanged();
}


void FTaskEditor::UpdateMaterialAfterGraphChange()
{
	TaskObject->Graph->LinkTaskSystemExpressionsFromGraph();
	TaskObject->MarkPackageDirty();
}


void FTaskEditor::DeleteSelectedDuplicatableNodes()
{
	// Cache off the old selection
	const FGraphPanelSelectionSet OldSelectedNodes = GraphEditor->GetSelectedNodes();

	// Clear the selection and only select the nodes that can be duplicated
	FGraphPanelSelectionSet RemainingNodes;
	GraphEditor->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(OldSelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if ((Node != NULL) && Node->CanDuplicateNode())
		{
			GraphEditor->SetNodeSelection(Node, true);
		}
		else
		{
			RemainingNodes.Add(Node);
		}
	}

	// Delete the duplicatable nodes
	DeleteSelectedNodes();

	// Reselect whatever's left from the original selection after the deletion
	GraphEditor->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(RemainingNodes); SelectedIter; ++SelectedIter)
	{
		if (UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter))
		{
			GraphEditor->SetNodeSelection(Node, true);
		}
	}
}

#undef LOCTEXT_NAMESPACE