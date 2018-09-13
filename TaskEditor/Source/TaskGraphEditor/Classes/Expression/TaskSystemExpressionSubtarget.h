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

// 对话子目标
USTRUCT()
struct FSubtargetDialog
{
public:
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, meta = (DisplayName = "NPC ID"))
		uint32 NpcID;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "对话 ID"))
		uint32 DialogID;
};

// 杀怪子目标
USTRUCT()
struct FSubtargetKillMonster
{
public:
	GENERATED_BODY()
		UPROPERTY(EditAnywhere, meta = (DisplayName = "怪物 ID"))
		uint32 MonsterID;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "怪物数量"))
		uint32 MonsterNum;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "怪物血量"))
		uint32 MonsterHealth;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "怪物掉落物品ID"))
		uint32 DropItemID;
};

// 到达子目标
USTRUCT()
struct FSubtargetReachPlace
{
public:
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (DisplayName = "坐标表ID"))
	uint32 PositionID;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "跟随友方怪ID"))
	uint32 FollowMonsterID;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "跟随冒泡"))
	uint32 FollowBubble;
};

USTRUCT()
struct FSubtargetRecycleItems
{
public:
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (DisplayName = "回收道具编号"))
	uint32 RecycleItemID;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "回收道具数量"))
	uint32 RecycleItemNum;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "来源类型"))
	uint32 SourceType;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "道具来源参数"))
	uint32 SourceParam;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "掉落ID"))
	uint32 DropItemID;
};

USTRUCT()
struct FSubtargetHandUpItems
{
public:
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (DisplayName = "上交道具编号"))
	uint32 UpItemID;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "上交道具数量"))
	uint32 UpItemNum;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "来源类型"))
	uint32 SourceType;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "道具来源参数"))
	uint32 SourceParam;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "掉落ID"))
	uint32 DropItemID;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "上交对象类型"))
	uint32 HandUpTargetType;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "上交对象ID"))
	uint32 HandUpTargetID;
};

USTRUCT()
struct FSubtargetGetItems
{
public:
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (DisplayName = "回收道具编号"))
	uint32 GetItemID;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "回收道具数量"))
	uint32 GetItemNum;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "道具来源类型"))
	uint32 SourceType;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "道具来源参数"))
	uint32 SourceParam;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "掉落ID"))
	uint32 DropItemID;
};


USTRUCT()
struct FSubtargetUseItems
{
public:
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (DisplayName = "回收道具编号"))
	uint32 UseItemID;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "回收道具数量"))
	uint32 UseItemNum;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "使用物品对象类型"))
	uint32 UseItemTargetType;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "使用物品对象ID"))
	uint32 UseItemTargetID;
};

USTRUCT()
struct FSubtargetDungeon
{
public:
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (DisplayName = "副本ID"))
	uint32 DungeonID;
};

USTRUCT()
struct FSubtargetReachLevel
{
public:
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (DisplayName = "到达等级"))
	uint32 ReachLevel;
};

USTRUCT()
struct FSubtargetCompleteActivity
{
public:
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (DisplayName = "活动ID"))
	uint32 CompleteActivityID;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "活动次数"))
	uint32 CompleteActivityNum;
};

USTRUCT()
struct FSubtargetAddFriendliness
{
public:
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (DisplayName = "类型"))
	uint32 AddType;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "增加点数"))
	uint32 AddNum;
};


USTRUCT()
struct FSubtargetOpenTreasureBox
{
public:
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (DisplayName = "宝箱ID"))
	uint32 TreasureBoxID;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "宝箱次数"))
	uint32 TreasureBoxNum;
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

	UPROPERTY(EditAnywhere, Category = "子目标", meta = (DisplayName = "杀怪子目标"))
		TArray<FSubtargetKillMonster> KillMonsters;

	UPROPERTY(EditAnywhere, Category = "子目标", meta = (DisplayName = "到达位置子目标"))
		TArray<FSubtargetReachPlace> ReachPlace;

	UPROPERTY(EditAnywhere, Category = "子目标", meta = (DisplayName = "回收物品子目标"))
		TArray<FSubtargetRecycleItems> RecycleItems;

	UPROPERTY(EditAnywhere, Category = "子目标", meta = (DisplayName = "上交物品子目标"))
		TArray<FSubtargetHandUpItems> HandUpItems;

	UPROPERTY(EditAnywhere, Category = "子目标", meta = (DisplayName = "获得物品子目标"))
		TArray<FSubtargetGetItems> GetItems;

	UPROPERTY(EditAnywhere, Category = "子目标", meta = (DisplayName = "使用物品子目标"))
		TArray<FSubtargetUseItems> UseItems;

	UPROPERTY(EditAnywhere, Category = "子目标", meta = (DisplayName = "刷副本子目标"))
		TArray<FSubtargetDungeon> Dungeons;

	UPROPERTY(EditAnywhere, Category = "子目标", meta = (DisplayName = "到达等级子目标"))
		TArray<FSubtargetReachLevel> ReachLevel;

	UPROPERTY(EditAnywhere, Category = "子目标", meta = (DisplayName = "完成活动子目标"))
		FSubtargetCompleteActivity CompleteActivities[3];

	UPROPERTY(EditAnywhere, Category = "子目标", meta = (DisplayName = "增加友好度子目标"))
		FSubtargetAddFriendliness AddFriendlinesses[1];

	UPROPERTY(EditAnywhere, Category = "子目标", meta = (DisplayName = "开宝箱子目标"))
		TArray<FSubtargetOpenTreasureBox> OpenTreasureBoxes;

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
