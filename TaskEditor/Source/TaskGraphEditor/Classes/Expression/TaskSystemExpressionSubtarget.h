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



UCLASS(MinimalAPI, DisplayName = "子目标 - 对话任务")
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

	UPROPERTY(EditAnywhere, meta = (DisplayName = "任务ID"))
	int32 TaskID;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "任务名字"))
	FString TaskName;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "任务类型"))
	TEnumAsByte<enum ESubtargetType> TargetType;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "任务描述"))
	FText TaskDesc;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "任务接受描述"))
	FText TaskAcceptDesc;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "任务进行中描述"))
	FText TaskDoingDesc;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "任务完成描述"))
	FText TaskFinishDesc;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "开始NPC ID"))
	int32 StartNpcID;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "接受对话ID"))
	int32 AcceptDialogID;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "完成NPC ID"))
	int32 FinishNpcID;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "完成对话ID"))
	int32 FinishDialogID;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "能否自动接受"))
	uint32 bCanAutoAccept : 1;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "能否重复接受"))
	uint32 bCanReAccept : 1;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "失败自动删除"))
	uint32 bRemoveIfFailed : 1;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "显示在任务栏中"))
	uint32 bDisplayInTaskColumn : 1;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "自动寻路"))
	uint32 bAutoFindWay : 1;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "限时任务"))
	uint32 bLimitTime : 1;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "限区域任务"))
	uint32 bLimitArea : 1;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "限职业"))
	uint32 bOccupationLimit : 1;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "限性别"))
	uint32 bSexLimit : 1;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "最小等级"))
	int32 LvLimitStart;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "最大等级"))
	int32 LvLimitEnd;

#if WITH_EDITOR
	virtual void GetCaption(TArray<FString>& OutCaptions) const override {
		if (!TaskName.IsEmpty()) {
			OutCaptions.Add(TEXT("   ") + TaskName);
		}

		const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("ESubtargetType"), true);
		check(EnumPtr);
		FText TypeStr = EnumPtr->GetDisplayNameTextByIndex(TargetType);
		FString Title = FString::Printf(TEXT("子目标 - %s                    "), *TypeStr.ToString());
		OutCaptions.Add(Title);
		
		
	}



#endif

};