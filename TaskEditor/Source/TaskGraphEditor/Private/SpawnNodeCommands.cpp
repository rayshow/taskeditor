#include"SpawnNodeCommands.h"

#define LOCTEXT_NAMESPACE "TaskGraphEditor"


TSharedPtr< FEdGraphSchemaAction > 
FSpawnNewTaskSystemNodeCommands::GetGraphActionByChord(FInputChord& InChord, UEdGraph* InDestGraph) const
{
	if (InChord.IsValidChord())
	{
		for (int32 x = 0; x < NodeCommands.Num(); ++x)
		{
			if (NodeCommands[x]->CommandInfo->HasActiveChord(InChord))
			{
				return NodeCommands[x]->GetAction(InDestGraph);
			}
		}
	}

	return TSharedPtr< FEdGraphSchemaAction >();
}

const TSharedPtr<const FInputChord> 
FSpawnNewTaskSystemNodeCommands::GetChordByClass(UClass* ExpressionClass) const
{

	for (int32 Index = 0; Index < NodeCommands.Num(); ++Index)
	{
		if (NodeCommands[Index]->GetExpressionClass() == ExpressionClass 
			&& NodeCommands[Index]->CommandInfo->GetFirstValidChord()->IsValidChord())
		{
			// Just return the first valid chord
			return NodeCommands[Index]->CommandInfo->GetFirstValidChord();
		}
	}

	return TSharedPtr< const FInputChord >();
}

#undef LOCTEXT_NAMESPACE
