#pragma once

#include"CoreMinimal.h"
#include"UObject/ObjectMacros.h"
#include"TaskSystemExpression.h"
#include"TaskSystemExpressionInput.h"
#include"TaskSystemExpressionSubtarget.generated.h"

UENUM()
enum ESubtargetType
{
	EST_KillingMonster = 0 UMETA(DisplayName = "杀怪任务"),
	EST_Collect UMETA(DisplayName = "采集任务"),
	EST_Dialog UMETA(DisplayName = "对话任务"),
	EST_UseItem UMETA(DisplayName = "使用物品任务"),
	EST_GetItem UMETA(DisplayName = "获得物品任务"),
	EST_AINodeComplete UMETA(DisplayName = "AI节点完成任务"),
	EST_ReachPosition UMETA(DisplayName = "到达任务"),
	EST_OpenChest UMETA(DisplayName = "开宝箱任务"),
	EST_HandleItem UMETA(DisplayName = "上交物品任务"),
	EST_CompleteDungeon UMETA(DisplayName = "完成副本任务"),
	EST_CompleteActivity UMETA(DisplayName = "完成活动任务"),
};


USTRUCT()
struct FSubtargetDialog
{
public:
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, meta = (DisplayName = "NPC ID"))
	int NpcID;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "对话 ID"))
	int DialogID;
};


UCLASS(MinimalAPI, DisplayName = "子目标(选择资源) - 可定制任务")
class UTaskSystemExpressionSubtarget : public UTaskSystemExpression
{
public:
	GENERATED_UCLASS_BODY()

	//Thread Input
	UPROPERTY()
	FTaskSystemExpressionInput Input;

	UPROPERTY()
	FTaskSystemExpressionInput BeforeAccess;

	UPROPERTY()
	FTaskSystemExpressionInput AfterAccess;

	UPROPERTY()
	FTaskSystemExpressionInput BeforePost;

	UPROPERTY()
	FTaskSystemExpressionInput AfterPost;


	UPROPERTY(EditAnywhere, Category = "通用参数", meta = (DisplayName = "能否自动接受"))
	uint32 bCanAutoAccept : 1;

	UPROPERTY(EditAnywhere, Category = "通用参数", meta = (DisplayName = "能否重复接受"))
		uint32 bCanReAccept : 1;

	UPROPERTY(EditAnywhere, Category = "通用参数", meta = (DisplayName = "失败自动删除"))
		uint32 bRemoveIfFailed : 1;

	UPROPERTY(EditAnywhere, Category = "通用参数", meta = (DisplayName = "显示在任务栏中"))
		uint32 bDisplayInTaskColumn : 1;

	UPROPERTY(EditAnywhere, Category = "通用参数", meta = (DisplayName = "是否自动寻路"))
		uint32 bAutoFindWay : 1;

	UPROPERTY(EditAnywhere, Category = "通用参数", meta = (DisplayName = "是否限时任务"))
		uint32 bLimitTime : 1;

	UPROPERTY(EditAnywhere, Category = "通用参数", meta = (DisplayName = "是否限区域任务"))
		uint32 bLimitArea : 1;

	UPROPERTY(EditAnywhere, Category = "通用参数", meta = (DisplayName = "是否限职业"))
		uint32 bLimitOccupation : 1;

	UPROPERTY(EditAnywhere, Category = "通用参数", meta = (DisplayName = "是否限性别"))
		uint32 bLimitSex : 1;

	UPROPERTY(EditAnywhere, Category = "通用参数", meta = (DisplayName = "是否限等级", InlineEditConditionToggle))
		uint32 bLimitLv : 1;

	UPROPERTY(EditAnywhere, Category = "通用参数", meta = (DisplayName = "等级范围", editcondition = "bLimitLv"))
		FIntPoint LvLimitStart;

	UPROPERTY(EditAnywhere, Category = "任务描述", meta = (DisplayName = "任务ID"))
		int32 TaskID;

	UPROPERTY(EditAnywhere, Category = "任务描述", meta = (DisplayName = "任务标题",MultiLine))
		FString TaskTitle;

	UPROPERTY(EditAnywhere, Category = "任务描述", meta = (DisplayName = "任务描述", MultiLine))
		FText TaskDesc;

	UPROPERTY(EditAnywhere, Category = "任务描述", meta = (DisplayName = "任务接受描述", MultiLine))
		FText TaskAcceptDesc;

	UPROPERTY(EditAnywhere, Category = "任务描述", meta = (DisplayName = "任务进行描述", MultiLine))
		FText TaskDoingDesc;

	UPROPERTY(EditAnywhere, Category = "任务描述", meta = (DisplayName = "任务完成描述", MultiLine))
		FText TaskFinishDesc;

	UPROPERTY()
		int32 StartNpcID;

	UPROPERTY(EditAnywhere, Category = "任务描述", meta = (DisplayName = "接受对话ID"))
		int32 AcceptDialogID;

	UPROPERTY()
		int32 FinishNpcID;

	UPROPERTY(EditAnywhere, Category = "任务描述", meta = (DisplayName = "完成对话ID"))
		int32 FinishDialogID;

	UPROPERTY(EditAnywhere, Category = "子目标", meta = (DisplayName = "对话子目标"))
		TArray<FSubtargetDialog> Dialogs;


	virtual void GetCaption(TArray<FString>& OutCaptions) const override {
		if (!TaskTitle.IsEmpty()) {
			OutCaptions.Add(TEXT("   ") + TaskTitle);
		}
		FString Title = TEXT("子任务                        ");
		OutCaptions.Add(Title);
	}


	virtual TArray<FName> GetCategroy()
	{
		static TArray<FName> Categories;
		if (Categories.Num() == 0)
		{
			Categories.Add(UTaskSystemGraphSchema::PC_All);
			Categories.Add(UTaskSystemGraphSchema::PC_TaskSubtarget);
		}
		return Categories;
	}

};
