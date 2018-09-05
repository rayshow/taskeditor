#pragma once

#include"CoreMinimal.h"
#include"UObject/ObjectMacros.h"
#include"TaskSystemExpression.h"
#include"TaskSystemExpressionInput.h"
#include"TaskSystemExpressionAbs.generated.h"


UCLASS(MinimalAPI, collapsecategories, hidecategories = Object)
class UTaskSystemExpressionAbs : public UTaskSystemExpression
{
	GENERATED_UCLASS_BODY()

	UPROPERTY()
	FTaskSystemExpressionInput Input;

#if WITH_EDITOR
	virtual void GetCaption(TArray<FString>& OutCaptions) const override {
		OutCaptions.Add(TEXT("任务"));
		OutCaptions.Add(TEXT("任务2"));
	}
#endif
};