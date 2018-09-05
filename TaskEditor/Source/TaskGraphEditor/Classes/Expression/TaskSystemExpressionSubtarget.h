#pragma once

#include"CoreMinimal.h"
#include"UObject/ObjectMacros.h"
#include"TaskSystemExpression.h"
#include"TaskSystemExpressionInput.h"
#include"TaskSystemExpressionSubtarget.generated.h"

UENUM()
enum ESubtargetType
{
	EST_KillingMonster = 0 UMETA(DisplayName = "杀怪"),
	EST_Collect UMETA(DisplayName = "采集"),
	EST_Dialog UMETA(DisplayName = "对话"),
	EST_UseItem UMETA(DisplayName = "使用物品"),
	EST_GetItem UMETA(DisplayName = "获得物品"),
	EST_AINodeComplete UMETA(DisplayName = "AI节点完成"),
	EST_ReachPosition UMETA(DisplayName = "到达位置"),
	EST_OpenChest UMETA(DisplayName = "开宝箱"),
	EST_HandleItem UMETA(DisplayName = "上交物品"),
	EST_CompleteDungeon UMETA(DisplayName = "完成副本"),
	EST_CompleteActivity UMETA(DisplayName = "完成活动"),
};



UCLASS(MinimalAPI, DisplayName = "子目标任务")
class UTaskSystemExpressionSubtarget : public UTaskSystemExpression
{
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

	UPROPERTY(EditAnywhere)
	int32 TaskID;

	UPROPERTY(EditAnywhere)
	FString TaskName;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<enum ESubtargetType> TargetType;

	UPROPERTY(EditAnywhere)
	FString TaskDesc;

	UPROPERTY(EditAnywhere)
	FString TaskAcceptDesc;

	UPROPERTY(EditAnywhere)
	FString TaskDoingDesc;

	UPROPERTY(EditAnywhere)
	FString TaskFinishDesc;

	UPROPERTY(EditAnywhere)
	int32 StartNpcID;

	UPROPERTY(EditAnywhere)
	int32 AcceptDialogID;

	UPROPERTY(EditAnywhere)
	int32 FinishNpcID;

	UPROPERTY(EditAnywhere)
	int32 FinishDialogID;

	UPROPERTY(EditAnywhere, Category = MaterialExpressionNoise)
	uint32 bCanAutoAccept : 1;

	UPROPERTY(EditAnywhere)
	uint32 bCanReAccept : 1;

	UPROPERTY(EditAnywhere)
	uint32 bRemoveIfFailed : 1;

	UPROPERTY(EditAnywhere)
	uint32 bDisplayInTaskColumn : 1;

	UPROPERTY(EditAnywhere)
	uint32 bAutoFindWay : 1;

	UPROPERTY(EditAnywhere)
	uint32 bLimitTime : 1;

	UPROPERTY(EditAnywhere)
	uint32 bLimitArea : 1;

	UPROPERTY(EditAnywhere)
	uint32 bOccupationLimit : 1;

	UPROPERTY(EditAnywhere)
	uint32 bSexLimit : 1;

	UPROPERTY(EditAnywhere)
	int32 LvLimitStart;

	UPROPERTY(EditAnywhere)
	int32 LvLimitEnd;

#if WITH_EDITOR
	virtual void GetCaption(TArray<FString>& OutCaptions) const override {
		OutCaptions.Add(TEXT("子任务"));
		OutCaptions.Add(TaskName);
	}



#endif

};