#pragma once

#include"CoreMinimal.h"
#include"UObject/ObjectMacros.h"
#include"TaskSystemExpression.h"
#include"TaskSystemExpressionInput.h"
#include"TaskSystemExpressionDialog.generated.h"

UCLASS(MinimalAPI, DisplayName = "2选1对话")
class UTaskSystemExpression1In2Dialog : public UTaskSystemExpression
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere)
	int32 DialogID;

	UPROPERTY()
	FText DialogContent;

	UPROPERTY()
	FTaskSystemExpressionInput Input;

	UPROPERTY()
	FText Yes;

	UPROPERTY()
	FText No;

#if WITH_EDITOR
	virtual void GetCaption(TArray<FString>& OutCaptions) const override {
		OutCaptions.Add(FString(TEXT("    2选1对话   ")));
	}
#endif

};