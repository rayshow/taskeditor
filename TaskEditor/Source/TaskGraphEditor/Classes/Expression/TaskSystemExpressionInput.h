#pragma once

#include "CoreMinimal.h"
#include "UObject/Class.h"
#include "SceneTypes.h"
#include "ObjectMacros.h"

UENUM()
enum TaskSystemExpressionInputType{
	TIT_NPCEntry = 0,
	TIT_NPCEntry_Test,
	TIT_DialogEntry,
	TIT_EventInput
};



USTRUCT(noexport)
struct FTaskSystemExpressionInput
{
#if WITH_EDITORONLY_DATA
	UPROPERTY()
	class UTaskSystemExpression* Expression;
#endif

	UPROPERTY()
	int32 OutputIndex;

	UPROPERTY()
	FName InputName;

	UPROPERTY()
	FName ExpressionName;

	UPROPERTY()
	TEnumAsByte<TaskSystemExpressionInputType> InputType;

	FTaskSystemExpressionInput()
		: OutputIndex(0)
	{
#if WITH_EDITORONLY_DATA
		Expression = nullptr;
#endif
	}
 
	bool IsConnected() const
	{
#if WITH_EDITORONLY_DATA
		return (nullptr != Expression);
#else
		return ExpressionName != NAME_None;
#endif  
	}

#if WITH_EDITOR 
	void Connect(int32 InOutputIndex, class UTaskSystemExpression* InExpression);
#endif  
	 
	bool Serialize(FArchive& Ar);

	FTaskSystemExpressionInput GetTracedInput() const;

	FName InputEnumToName();

	FName InputSubEnumToName();

};

template<>
struct TStructOpsTypeTraits<FTaskSystemExpressionInput>
	: public TStructOpsTypeTraitsBase2<FTaskSystemExpressionInput>
{
	enum
	{
		WithSerializer = true,
	};
};
