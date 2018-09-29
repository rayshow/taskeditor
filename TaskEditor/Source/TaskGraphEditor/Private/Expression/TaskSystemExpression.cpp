#include"Expression/TaskSystemExpression.h"
#include"Expression/TaskSystemExpressionInput.h"
#include"TaskSystemGraphSchema.h"
#include"UObject/UnrealType.h"

#define LOCTEXT_NAMESPACE "TaskGraphEditor"
const static FName TaskSystemInputName("TaskSystemExpressionInput");


UTaskSystemExpression::UTaskSystemExpression(FObjectInitializer const& Initializer)
	:UObject(Initializer) {}



const TArray<FTaskSystemExpressionInput*> UTaskSystemExpression::GetInputs()
{
	static const FName NAME_InputName("InputName");
	TMap<FString, bool> Enables;
	for (TFieldIterator<UBoolProperty> InputIt(GetClass(), EFieldIteratorFlags::IncludeSuper, EFieldIteratorFlags::ExcludeDeprecated); InputIt; ++InputIt)
	{
		UBoolProperty*  boolProp = *InputIt;
		if ( !boolProp->GetMetaData(NAME_InputName).IsEmpty())
		{
			for (int32 ArrayIndex = 0; ArrayIndex < boolProp->ArrayDim; ArrayIndex++)
			{
				Enables.Add(boolProp->GetMetaData(NAME_InputName),
					*boolProp->ContainerPtrToValuePtr<bool>(this, ArrayIndex));
			}
		}
	}

	TArray<FTaskSystemExpressionInput*> Result;
	TArray<FTaskSystemExpressionInput*> Events;
	for (TFieldIterator<UStructProperty> InputIt(GetClass(), EFieldIteratorFlags::IncludeSuper, EFieldIteratorFlags::ExcludeDeprecated); InputIt; ++InputIt)
	{
		UStructProperty* StructProp = *InputIt;
		if (StructProp->Struct->GetFName() == TaskSystemInputName)
		{
			FString Name = StructProp->GetName();
		
			for (int32 ArrayIndex = 0; ArrayIndex < StructProp->ArrayDim; ArrayIndex++)
			{
				auto Input = StructProp->ContainerPtrToValuePtr<FTaskSystemExpressionInput>(this, ArrayIndex);

				if (!Input->IsEvent) 
				{
					Result.Add(Input);
				}
				else if (Enables[StructProp->GetName()])
				{
					Events.Add(Input);
				}
			}
		}
	}
	Result.Append(Events);
	return Result;
}


FTaskSystemExpressionInput* UTaskSystemExpression::GetInput(int32 InputIndex)
{
	int32 Index = 0;
	for (TFieldIterator<UStructProperty> InputIt(GetClass(), EFieldIteratorFlags::IncludeSuper, EFieldIteratorFlags::ExcludeDeprecated); InputIt; ++InputIt)
	{
		UStructProperty* StructProp = *InputIt;
		if (StructProp->Struct->GetFName() == NAME_ExpressionInput)
		{
			for (int32 ArrayIndex = 0; ArrayIndex < StructProp->ArrayDim; ArrayIndex++)
			{
				if (Index == InputIndex)
				{
					return StructProp->ContainerPtrToValuePtr<FTaskSystemExpressionInput>(this, ArrayIndex);
				}
				Index++;
			}
		}
	}

	return NULL;
}


FName UTaskSystemExpression::GetInputName(int32 InputIndex) const
{
	int32 Index = 0;
	for (TFieldIterator<UStructProperty> InputIt(GetClass(),
		EFieldIteratorFlags::IncludeSuper, EFieldIteratorFlags::ExcludeDeprecated); InputIt; ++InputIt)
	{
		UStructProperty* StructProp = *InputIt;
		if (StructProp->Struct->GetFName() == TaskSystemInputName)
		{
			for (int32 ArrayIndex = 0; ArrayIndex < StructProp->ArrayDim; ArrayIndex++)
			{
				if (Index == InputIndex)
				{
					FTaskSystemExpressionInput const* Input = 
						StructProp->ContainerPtrToValuePtr<FTaskSystemExpressionInput>(this, ArrayIndex);

					if (!Input->InputName.IsNone())
					{
						return Input->InputName;
					}
					else
					{
						FName StructName = StructProp->GetFName();

						if (StructProp->ArrayDim > 1)
						{
							StructName = *FString::Printf(TEXT("%s_%d"), *StructName.ToString(), ArrayIndex);
						}

						return StructName;
					}
				}
				Index++;
			}
		}
	}
	return NAME_None;
}

bool UTaskSystemExpression::IsInputConnectionRequired(int32 InputIndex) const { return true; }
bool UTaskSystemExpression::CanUserDeleteExpression() const { return true; };

uint32 UTaskSystemExpression::GetInputType(int32 InputIndex)
{
	return 0;
}

uint32 UTaskSystemExpression::GetOutputType(int32 OutputIndex) { return 0; }

FText UTaskSystemExpression::GetCreationDescription() const { return FText::GetEmpty(); }
FText UTaskSystemExpression::GetCreationName() const { return FText::GetEmpty(); }
int32 UTaskSystemExpression::GetWidth() const { return 96 + (8 * 2); }
int32 UTaskSystemExpression::GetHeight() const { return 18 + 3 * 21; }

void UTaskSystemExpression::GetCaption(TArray<FString>& OutCaptions) const { OutCaptions.Add(TEXT("Expression")); }

FString UTaskSystemExpression::GetDescription() const {
	TArray<FString> Captions;
	GetCaption(Captions);
	return Captions[0];
}

void UTaskSystemExpression::GetConnectorToolTip(int32 InputIndex, int32 OutputIndex, TArray<FString>& OutToolTip)
{
	OutToolTip.Add("ConnectorTooltip");
}

void UTaskSystemExpression::GetExpressionToolTip(TArray<FString>& OutToolTip) {
	OutToolTip.Add("Expression Tooltip");
}


int UTaskSystemExpression::GetLabelPadding() { return 0; }
int32 UTaskSystemExpression::CompilerError(class FMaterialCompiler* Compiler, const TCHAR* pcMessage)
{
	return 0;
}

bool UTaskSystemExpression::MatchesSearchQuery(const TCHAR* SearchQuery)
{
	return true;
}

void UTaskSystemExpression::ConnectExpression(FTaskSystemExpressionInput* Input, int32 OutputIndex)
{

	if (Input && OutputIndex >= 0 && OutputIndex < Outputs.Num())
	{
		FTaskSystemExpressionOutput& Output = Outputs[OutputIndex];
		Input->Expression = this;
		Input->OutputIndex = OutputIndex;
	}
}

bool UTaskSystemExpression::CanRenameNode() const
{
	return true;
}

FString UTaskSystemExpression::GetEditableName() const
{
	check(false);
	return TEXT("");
}

void UTaskSystemExpression::SetEditableName(const FString& NewName)
{
	check(false);
}
FName UTaskSystemExpression::GetParameterName() const { return NAME_None; }


bool UTaskSystemExpression::ContainsInputLoop(const bool bStopOnFunctionCall) 
{
	return false; 
}


#undef LOCTEXT_NAMESPACE