#pragma once

#include"Expression/TaskSystemExpressionClasses.h"
#include"Expression/TaskSystemExpressionComment.h"
#include"Expression/TaskSystemExpression.h"
#include"Expression/TaskSystemExpressionDialog.h"
#include"Expression/TaskSystemExpressionSubtarget.h"
#include"Expression/TaskSystemExpressionWeather.h"
#include"Expression/TaskSystemExpressionSpawnMonster.h"
#include"TaskSystemGraphSchema.h"
#include"UObject/Class.h"
#include"ObjectMacros.h"
#include"Casts.h"
#include"Package.h"
#include"UObjectIterator.h"
#include"UnrealType.h"

#define LOCTEXT_NAMESPACE "TaskGraphEditor"

TaskSystemExpressionClasses* TaskSystemExpressionClasses::Get()
{
	static TaskSystemExpressionClasses Inst;
	Inst.InitializeExpressionClasses();
	return &Inst;
}

void TaskSystemExpressionClasses::AddExpressions(FGraphActionMenuBuilder& ActionMenuBuilder,
	FText CategoryName, TArray<FTaskSystemExpression>* Expressions)
{
	for (int32 Index = 0; Index < Expressions->Num(); ++Index)
	{
		const FTaskSystemExpression& Expression = (*Expressions)[Index];
		
		FText CreationName = FText::FromString(Expression.Name);
		FFormatNamedArguments Arguments;
		Arguments.Add(TEXT("Name"), CreationName);
		FText ToolTip = FText::Format(LOCTEXT("NewMaterialExpressionTooltip", "Adds a {Name} node here"), Arguments);
		if (!Expression.CreationDescription.IsEmpty())
		{
			ToolTip = Expression.CreationDescription;
		}
		if (!Expression.CreationName.IsEmpty())
		{
			CreationName = Expression.CreationName;
		}

		TSharedPtr<FTaskSystemGraphSchemaAction_NewNode> NewNodeAction(new FTaskSystemGraphSchemaAction_NewNode(
			CategoryName,
			CreationName,
			ToolTip, 0, CastChecked<UTaskSystemExpression>(Expression.Class->GetDefaultObject())->GetKeywords()));
		NewNodeAction->TaskSystemExpressionClass = Expression.Class;
		ActionMenuBuilder.AddAction(NewNodeAction);
	}
}


void TaskSystemExpressionClasses::InitializeExpressionClasses()
{
	UClass* BaseType = UTaskSystemExpression::StaticClass();

	static const UStruct* ExpressionInputStruct =
		CastChecked<UStruct>(StaticFindObject(UStruct::StaticClass(), ANY_PACKAGE, TEXT("ExpressionInput")));
	check(ExpressionInputStruct);
	TArray<UStructProperty*>	ExpressionInputs;

	AllExpressionClasses.Empty();
	if (BaseType)
	{
		for (TObjectIterator<UClass> It; It; ++It)
		{
			UClass* Class = *It;
			if (!Class->HasAnyClassFlags(CLASS_Abstract | CLASS_Deprecated)
				&& Class->IsChildOf(BaseType))
			{
				ExpressionInputs.Empty();

				if (Class != UTaskSystemExpressionComment::StaticClass())
				{
					FTaskSystemExpression Expression;
					
					static const FString ExpressionPrefix = TEXT("TaskSystemExpression");
					FString ClassName = *Class->GetName();

					if (Class->HasMetaData("DisplayName"))
					{
						ClassName = Class->GetDisplayNameText().ToString();
					}

					if (ClassName.StartsWith(ExpressionPrefix, ESearchCase::CaseSensitive))
					{
						ClassName = ClassName.Mid(ExpressionPrefix.Len());
					}
					Expression.Name = ClassName;
					Expression.Class = Class;
					UTaskSystemExpression* TempObject = Cast<UTaskSystemExpression>(Class->GetDefaultObject());
					if (TempObject)
					{
						Expression.CreationDescription = TempObject->GetCreationDescription();
						Expression.CreationName = TempObject->GetCreationName();
					}

					AllExpressionClasses.Add(Expression);

					// Initialize the expression class input map.							
					for (TFieldIterator<UStructProperty> InputIt(Class); InputIt; ++InputIt)
					{
						UStructProperty* StructProp = *InputIt;
						if (StructProp->Struct == ExpressionInputStruct)
						{
							ExpressionInputs.Add(StructProp);
						}
					}
				}
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE