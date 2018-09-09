#pragma once

#include"CoreMinimal.h"
#include"UObject/ObjectMacros.h"
#include"TaskSystemExpression.h"
#include"TaskSystemExpressionInput.h"
#include"TaskSystemExpressionSpawnMonster.generated.h"

UENUM()
enum EMonsterEventType
{
	EMT_SpawnMonster = 0 UMETA(DisplayName = "招怪"),
	EMT_RecycleMonster UMETA(DisplayName = "回收怪"),
};

UCLASS(MinimalAPI, DisplayName = " 事件 - 招怪")
class UTaskSystemExpressionSpawnMonster : public UTaskSystemExpression
{
public:
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, meta=(DisplayName="事件名字"))
	FString EventName;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "怪物ID"))
	int32 MonsterID;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "怪物数量"))
	int32 Count;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "描述"))
	FText MonsterDesc;

	virtual void GetCaption(TArray<FString>& OutCaptions) const override {
		static FString SSpawnMonster(TEXT(" 招怪 "));
		static FString SRecycleMonster(TEXT(" 回收怪 "));
		OutCaptions.Add(FString(TEXT(" 事件 - 招怪 ")) );
		if (!EventName.IsEmpty()) {
			OutCaptions.Add(EventName);
		}
	}

	virtual TArray<FName> GetCategroy()
	{
		static TArray<FName> Categories;
		if (Categories.Num() == 0)
		{
			Categories.Add(UTaskSystemGraphSchema::PC_All);
			Categories.Add(UTaskSystemGraphSchema::PC_Event);
		}
		return Categories;
	}
};