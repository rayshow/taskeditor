#pragma once

#include"CoreMinimal.h"

struct FTaskSystemExpression
{
	FString Name;
	UClass* Class;
	FText CreationDescription;
	FText CreationName;

	friend bool operator==(const FTaskSystemExpression& A, const FTaskSystemExpression& B)
	{
		return A.Class == B.Class;
	}
};


class TaskSystemExpressionClasses
{
public:
	static TaskSystemExpressionClasses * Get();

	void AddExpressions(struct FGraphActionMenuBuilder& ActionMenuBuilder,
		FText CategoryName, TArray<FTaskSystemExpression>* Expressions); \

	void InitializeExpressionClasses();

	TArray<FTaskSystemExpression> AllExpressionClasses;

};