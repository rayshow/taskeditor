#pragma once

#include"CoreMinimal.h"
#include"UObject/ObjectMacros.h"
#include"TaskSystemExpression.h"
#include"TaskSystemExpressionInput.h"
#include"TaskSystemExpressionMonsterEvent.generated.h"

UENUM()
enum EMonsterEventType
{
	EMT_SpawnMonster = 0 UMETA(DisplayName = "招怪"),
	EMT_RecycleMonster UMETA(DisplayName = "回收怪"),
};

UCLASS(MinimalAPI, DisplayName = "招怪")
class UTaskSystemExpressionMonsterEvent : public UTaskSystemExpression
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere)
	FString EventName;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<enum EMonsterEventType> EventType;

	UPROPERTY(EditAnywhere)
	int32 MonsterID;

	UPROPERTY(EditAnywhere)
	FString MonsterDesc;

#if WITH_EDITOR
	virtual void GetCaption(TArray<FString>& OutCaptions) const override {
		static FString SSpawnMonster(TEXT(" 招怪 "));
		static FString SRecycleMonster(TEXT(" 回收怪 "));
		OutCaptions.Add(FString(TEXT("  子任务 - ")+ 
			(EventType== EMT_SpawnMonster? SSpawnMonster:SRecycleMonster)));
		OutCaptions.Add(EventName);
	}
#endif

};