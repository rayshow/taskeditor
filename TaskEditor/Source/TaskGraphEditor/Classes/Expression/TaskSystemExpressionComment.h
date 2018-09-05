#pragma once

#include"CoreMinimal.h"
#include"ObjectMacros.h"
#include"TaskSystemExpression.h"
#include"TaskSystemExpressionComment.generated.h"

UCLASS(MinimalAPI, DisplayName = "注释")
class UTaskSystemExpressionComment : public UTaskSystemExpression
{
	GENERATED_UCLASS_BODY()

	UPROPERTY()
	int32 SizeX;

	UPROPERTY()
	int32 SizeY;

	UPROPERTY(EditAnywhere, Category = MaterialExpressionComment, meta = (MultiLine = true))
	FString Text;

	/** Color to style comment with */
	UPROPERTY(EditAnywhere, Category = MaterialExpressionComment)
	FLinearColor CommentColor;

	/** Size of the text in the comment box */
	UPROPERTY(EditAnywhere, Category = MaterialExpressionComment, meta = (ClampMin = 1, ClampMax = 1000))
	int32 FontSize;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void GetCaption(TArray<FString>& OutCaptions) const override{ OutCaptions.Add(TEXT("Comment")); }
	virtual bool MatchesSearchQuery(const TCHAR* SearchQuery) override {
		if (Text.Contains(SearchQuery)) {
			return true;
		}
		return false;
	}
#endif // WITH_EDITOR

};


