#include"TaskSystemExpression.h"
#include"TaskSystemExpressionInput.h"
#include"UObject/UnrealType.h"

#define LOCTEXT_NAMESPACE "TaskGraphEditor"

UTaskSystemExpression::UTaskSystemExpression(FObjectInitializer const& Initializer)
	:UObject(Initializer) {}

const static FName TaskSystemInputName("TaskSystemExpressionInput");

const TArray<FTaskSystemExpressionInput*> UTaskSystemExpression::GetInputs()
{
	TArray<FTaskSystemExpressionInput*> Result;
	for (TFieldIterator<UStructProperty> InputIt(GetClass(), EFieldIteratorFlags::IncludeSuper, EFieldIteratorFlags::ExcludeDeprecated); InputIt; ++InputIt)
	{
		UStructProperty* StructProp = *InputIt;
		if (StructProp->Struct->GetFName() == TaskSystemInputName)
		{
			for (int32 ArrayIndex = 0; ArrayIndex < StructProp->ArrayDim; ArrayIndex++)
			{
				Result.Add(StructProp->ContainerPtrToValuePtr<FTaskSystemExpressionInput>(this, ArrayIndex));
			}
		}
	}
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
//
//static void CopyMaterialExpressions(
//	const TArray<class UMaterialExpression*>& SrcExpressions,
//	const TArray<class UMaterialExpressionComment*>& SrcExpressionComments,
//	class UMaterial* Material, class UMaterialFunction* Function,
//	TArray<class UMaterialExpression*>& OutNewExpressions,
//	TArray<class UMaterialExpression*>& OutNewComments)
//{
//	OutNewExpressions.Empty();
//	OutNewComments.Empty();
//
//	UObject* ExpressionOuter = Material;
//	if (EditFunction)
//	{
//		ExpressionOuter = EditFunction;
//	}
//
//	TMap<UMaterialExpression*, UMaterialExpression*> SrcToDestMap;
//
//	// Duplicate source expressions into the editor's material copy buffer.
//	for (int32 SrcExpressionIndex = 0; SrcExpressionIndex < SrcExpressions.Num(); ++SrcExpressionIndex)
//	{
//		UMaterialExpression*	SrcExpression = SrcExpressions[SrcExpressionIndex];
//		UMaterialExpressionMaterialFunctionCall* FunctionExpression = Cast<UMaterialExpressionMaterialFunctionCall>(SrcExpression);
//		bool bIsValidFunctionExpression = true;
//
//		if (EditFunction
//			&& FunctionExpression
//			&& FunctionExpression->MaterialFunction
//			&& FunctionExpression->MaterialFunction->IsDependent(EditFunction))
//		{
//			bIsValidFunctionExpression = false;
//		}
//
//		if (bIsValidFunctionExpression && IsAllowedExpressionType(SrcExpression->GetClass(), EditFunction != NULL))
//		{
//			UMaterialExpression* NewExpression = Cast<UMaterialExpression>(StaticDuplicateObject(SrcExpression, ExpressionOuter, NAME_None, RF_Transactional));
//			NewExpression->Material = Material;
//			// Make sure we remove any references to functions the nodes came from
//			NewExpression->Function = NULL;
//
//			SrcToDestMap.Add(SrcExpression, NewExpression);
//
//			// Add to list of material expressions associated with the copy buffer.
//			Material->Expressions.Add(NewExpression);
//
//			// There can be only one default mesh paint texture.
//			UMaterialExpressionTextureBase* TextureSample = Cast<UMaterialExpressionTextureBase>(NewExpression);
//			if (TextureSample)
//			{
//				TextureSample->IsDefaultMeshpaintTexture = false;
//			}
//
//			NewExpression->UpdateParameterGuid(true, true);
//			NewExpression->UpdateMaterialExpressionGuid(true, true);
//
//			UMaterialExpressionFunctionInput* FunctionInput = Cast<UMaterialExpressionFunctionInput>(NewExpression);
//			if (FunctionInput)
//			{
//				FunctionInput->ConditionallyGenerateId(true);
//				FunctionInput->ValidateName();
//			}
//
//			UMaterialExpressionFunctionOutput* FunctionOutput = Cast<UMaterialExpressionFunctionOutput>(NewExpression);
//			if (FunctionOutput)
//			{
//				FunctionOutput->ConditionallyGenerateId(true);
//				FunctionOutput->ValidateName();
//			}
//
//			// Record in output list.
//			OutNewExpressions.Add(NewExpression);
//		}
//	}
//
//	// Fix up internal references.  Iterate over the inputs of the new expressions, and for each input that refers
//	// to an expression that was duplicated, point the reference to that new expression.  Otherwise, clear the input.
//	for (int32 NewExpressionIndex = 0; NewExpressionIndex < OutNewExpressions.Num(); ++NewExpressionIndex)
//	{
//		UMaterialExpression* NewExpression = OutNewExpressions[NewExpressionIndex];
//		const TArray<FExpressionInput*>& ExpressionInputs = NewExpression->GetInputs();
//		for (int32 ExpressionInputIndex = 0; ExpressionInputIndex < ExpressionInputs.Num(); ++ExpressionInputIndex)
//		{
//			FExpressionInput* Input = ExpressionInputs[ExpressionInputIndex];
//			UMaterialExpression* InputExpression = Input->Expression;
//			if (InputExpression)
//			{
//				UMaterialExpression** NewInputExpression = SrcToDestMap.Find(InputExpression);
//				if (NewInputExpression)
//				{
//					check(*NewInputExpression);
//					Input->Expression = *NewInputExpression;
//				}
//				else
//				{
//					Input->Expression = NULL;
//				}
//			}
//		}
//	}
//
//	// Copy Selected Comments
//	for (int32 CommentIndex = 0; CommentIndex<SrcExpressionComments.Num(); CommentIndex++)
//	{
//		UMaterialExpressionComment* ExpressionComment = SrcExpressionComments[CommentIndex];
//		UMaterialExpressionComment* NewComment = Cast<UMaterialExpressionComment>(StaticDuplicateObject(ExpressionComment, ExpressionOuter));
//		NewComment->Material = Material;
//
//		// Add reference to the material
//		Material->EditorComments.Add(NewComment);
//
//		// Add to the output array.
//		OutNewComments.Add(NewComment);
//	}
//}

void UTaskSystemExpression::ConnectExpression(FTaskSystemExpressionInput* Input, int32 OutputIndex)
{

	if (Input && OutputIndex >= 0 && OutputIndex < Outputs.Num())
	{
		FTaskSystemExpressionOutput& Output = Outputs[OutputIndex];
		Input->Expression = this;
		Input->OutputIndex = OutputIndex;
		Input->Mask = Output.Mask;
		Input->MaskR = Output.MaskR;
		Input->MaskG = Output.MaskG;
		Input->MaskB = Output.MaskB;
		Input->MaskA = Output.MaskA;
	}

}
FText UTaskSystemExpression::GetKeywords() const { return FText::GetEmpty(); }

bool UTaskSystemExpression::GetAllInputExpressions(TArray<UTaskSystemExpression*>& InputExpressions)
{
	if (!InputExpressions.Contains(this))
	{
		InputExpressions.Add(this);
	}
	return false;
}


bool UTaskSystemExpression::CanRenameNode() const
{
	return true;
}
FString UTaskSystemExpression::GetEditableName() const
{
	// This function is only safe to call in a class that has implemented CanRenameNode() to return true
	check(false);
	return TEXT("");
}

void UTaskSystemExpression::SetEditableName(const FString& NewName)
{
	check(false);
}
FName UTaskSystemExpression::GetParameterName() const { return NAME_None; }


bool UTaskSystemExpression::ContainsInputLoop(const bool bStopOnFunctionCall) { return false; }

#undef LOCTEXT_NAMESPACE