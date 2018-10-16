#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "Misc/Guid.h"
#include "TaskSystemExpressionOutput.h"
#include "TaskSystemGraphSchema.h"
#include "TaskSystemExpression.generated.h"

class UEdGraphNode;
class UTaskObject;
struct FTaskSystemExpressionOutput;
struct FTaskSystemExpressionInput;

DECLARE_DELEGATE_OneParam(FOnExpressionChanged, FString);

UCLASS(abstract, BlueprintType, hidecategories = Object)
class UTaskSystemExpression: public UObject
{
public:
	GENERATED_UCLASS_BODY()

#if WITH_EDITORONLY_DATA

	UPROPERTY(transient)
	UEdGraphNode*	GraphNode;

#endif 

	UTaskObject* TaskObject;
	TArray<FTaskSystemExpressionOutput> Outputs;
	FOnExpressionChanged OnExpressionChanged;

#if WITH_EDITOR
	virtual TArray<FTaskSystemExpressionOutput>& GetOutputs() { return Outputs; }
	virtual const TArray<FTaskSystemExpressionInput*> GetInputs();
	virtual FTaskSystemExpressionInput* GetInput(int32 InputIndex);
	virtual FName GetInputName(int32 InputIndex) const;
	virtual bool IsInputConnectionRequired(int32 InputIndex) const;
	virtual bool CanUserDeleteExpression() const;
	virtual uint32 GetInputType(int32 InputIndex);
	virtual uint32 GetOutputType(int32 OutputIndex);

	virtual FText GetCreationDescription() const;
	virtual FText GetCreationName() const;
	virtual int32 GetWidth() const;
	virtual int32 GetHeight() const;

	virtual void GetCaption(TArray<FString>& OutCaptions) const;
	virtual FString GetDescription() const;
	virtual void GetConnectorToolTip(int32 InputIndex, int32 OutputIndex, TArray<FString>& OutToolTip);
	virtual void GetExpressionToolTip(TArray<FString>& OutToolTip);
	virtual int GetLabelPadding();
	virtual int32 CompilerError(class FMaterialCompiler* Compiler, const TCHAR* pcMessage);

	virtual bool MatchesSearchQuery(const TCHAR* SearchQuery);

	void ConnectExpression(FTaskSystemExpressionInput* Input, int32 OutputIndex);

	/** Returns the keywords that should be used when searching for this expression */
	virtual FText GetKeywords() const { return FText(); };
	virtual bool CanRenameNode() const;
	virtual FString GetEditableName() const;
	virtual void SetEditableName(const FString& NewName);
	virtual FName GetParameterName() const;
	virtual void SetParameterName(const FName& Name) {}
#endif //WITH_EDITOR
	bool ContainsInputLoop(const bool bStopOnFunctionCall = true);
	
	virtual TArray<FName> GetCategroy()
	{
		static TArray<FName> Categories;
		if(Categories.Num()==0)
		{
			Categories.Add(UTaskSystemGraphSchema::PC_All);
		}
		return Categories;
	}
};