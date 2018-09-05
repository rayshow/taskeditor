#pragma once

#include"CoreMinimal.h"
#include"UICommandInfo.h"
#include"EdGraph/EdGraphSchema.h"
#include"Expression/TaskSystemExpressionComment.h"
#include"TaskSystemGraphSchema.h"

class FTaskSystemExpressionSpawnInfo
{
public:
	FTaskSystemExpressionSpawnInfo(UClass* InTaskSystemExpressionClass)
		:TaskSystemExpressionClass{ InTaskSystemExpressionClass } {}

	TSharedPtr< FEdGraphSchemaAction > GetAction(UEdGraph* InGraph)
	{
		if (TaskSystemExpressionClass == UTaskSystemExpressionComment::StaticClass()) {
			return MakeShareable(new FTaskSystemGraphSchemaAction_NewComment);
		}
		else {
			TSharedPtr<FTaskSystemGraphSchemaAction_NewNode> NewNodeAction
				= MakeShareable(new FTaskSystemGraphSchemaAction_NewNode);
			NewNodeAction->TaskSystemExpressionClass = TaskSystemExpressionClass;
			return NewNodeAction;
		}
	}

	UClass* GetExpressionClass() { return TaskSystemExpressionClass; }

	TSharedPtr<FUICommandInfo> CommandInfo;

private:
	UClass * TaskSystemExpressionClass;
};


class FSpawnNewTaskSystemNodeCommands : public TCommands<FSpawnNewTaskSystemNodeCommands>
{
public:

	FSpawnNewTaskSystemNodeCommands()
		: TCommands<FSpawnNewTaskSystemNodeCommands>(
			TEXT("MaterialEditorSpawnNodes"), 
			NSLOCTEXT("Contexts", "MaterialEditor_SpawnNodes", "Material Editor - Spawn Nodes"),
			NAME_None, FEditorStyle::GetStyleSetName()) {}

	virtual void RegisterCommands() override
	{

	}

	TSharedPtr< FEdGraphSchemaAction > GetGraphActionByChord(FInputChord& InChord, UEdGraph* InDestGraph) const;

	const TSharedPtr<const FInputChord> GetChordByClass(UClass* MaterialExpressionClass) const;

private:
	/** An array of all the possible commands for spawning nodes */
	TArray< TSharedPtr< FTaskSystemExpressionSpawnInfo > > NodeCommands;
};