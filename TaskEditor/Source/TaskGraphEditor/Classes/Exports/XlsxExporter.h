#pragma once
#include "Excel.h"
#include "Object.h"

enum class ECELL_POS:uint8
{
	TaskID,
	TaskType,
	TaskName,
	TaskDesc,
	TaskAcceptDesc,
	TaskDoingDesc,
	TaskFinishDesc,
	TaskStartNpc,
	AcceptDialogID,
	TaskFinishNpc,
	FinishDialogID,
	PreTaskRelation,
	PreTaskID,
	NextTaskID,
	NextTaskRelation,
	NextTaskParam,
	IsAutoAccept,
	_1,
	IsTimeoutAutoGiveUp,
	IsGiveUpAutoReAccept,
	CanSubmitFromClient,
	IsAutoFinish,
	LimitProfession,
	LimitSex,
	LimitMaxLevel,
	LimitMinLevel,
	LimitTime,
	LimitMinTeamNumber,
	LimitMaxTeamNumber,
	_2,
	_3,
	RewardID0,
	RewardID1,
	RewardID2,
	RewardID3,
	RewardID4,
	AcceptCreateMonsterGroupID,
	_4,_5,_6,_7,
	Target0Type,
	Target0Param0,
	Target0Param1,
	Target0Param2,
	Target0Param3,
	Target0Desc,
	Target0ChangeDesc,
	Target1Type,
	Target1Param0,
	Target1Param1,
	Target1Param2,
	Target1Param3,
	Target1Desc,
	Target1ChangeDesc,
	MAX,
	TargetType = MAX,
	TargetParam0,
	TargetParam1,
	TargetParam2,
	TargetParam3,
	TargetDesc,
	TargetChangeDesc,
};

constexpr int EP_MaxGroup = 2;

static const int EP_TARGETS[][7] = {
	{ (int)ECELL_POS::Target0Type ,(int)ECELL_POS::Target0Param0,(int)ECELL_POS::Target0Param1,(int)ECELL_POS::Target0Param2,(int)ECELL_POS::Target0Param3,(int)ECELL_POS::Target0Desc, (int)ECELL_POS::Target0ChangeDesc },
	{ (int)ECELL_POS::Target1Type ,(int)ECELL_POS::Target1Param0,(int)ECELL_POS::Target1Param1,(int)ECELL_POS::Target1Param2,(int)ECELL_POS::Target1Param3,(int)ECELL_POS::Target1Desc, (int)ECELL_POS::Target1ChangeDesc }
};

static const FString EP_CELLS[][3] = {
{ "A%d" ,TEXT("任务ID"),   "m_stBaseInfo.m_iTaskID" },
{ "B%d" ,TEXT("任务类型"), "m_stBaseInfo.m_iTaskType" },
{ "C%d" ,TEXT("任务名字"), "m_stBaseInfo.m_szTaskName" },
{ "D%d" ,TEXT("任务描述"), "m_stBaseInfo.m_szTaskDesc" },
{ "E%d" ,TEXT("接受任务描述"), "m_stBaseInfo.m_szTaskDescAccept" },
{ "F%d" ,TEXT("任务进行中描述"), "m_stBaseInfo.m_szTaskDescDoing" },
{ "G%d" ,TEXT("任务完成描述"), "m_stBaseInfo.m_szTaskDescFinish" },
{ "H%d" ,TEXT("起始NPC"), "m_stBaseInfo.m_iStartNpc" },
{ "I%d" ,TEXT("起始NPC对话ID"), "m_stBaseInfo.m_iAcceptDialogID" },
{ "J%d" ,TEXT("完成NPC"), "m_stBaseInfo.m_iFinishNpc" },
{ "K%d" ,TEXT("完成NPC对话ID"), "m_stBaseInfo.m_iFinishDialogID" },
{ "L%d" ,TEXT("前置任务关系"), "m_stCondInfo.m_iPreTaskRelation" },
{ "M%d" ,TEXT("前置任务ID"), "m_stCondInfo.astPreTaskID[0]" },
{ "N%d" ,TEXT("后置任务"), "m_stEventInfo.m_astNextTaskID" },
{ "O%d" ,TEXT("后置任务关系"), "m_stEventInfo.m_iNexTaskRelation" },
{ "P%d" ,TEXT("后置任务参数"), "m_stEventInfo.m_astNextTaskParam" },
{ "Q%d" ,TEXT("是否自动领取任务"), "m_stFlagInfo.m_bIsAutoAccept" },
{ "R%d" ,TEXT("任务是否可放弃"), "" },
{ "S%d" ,TEXT("失败是否自动放弃"), "m_stFlagInfo.m_bTimeoutAutoGiveUp" },
{ "T%d" ,TEXT("放弃是否可重接"), "m_stFlagInfo.m_bGiveUpAutoReAccept" },
{ "U%d" ,TEXT("是否可通过前端直接提交"), "m_stFlagInfo.m_bCanSubmitFromClient" },
{ "V%d" ,TEXT("能否自动完成"), "m_stFlagInfo.m_bIsAutoFinish" },
{ "W%d" ,TEXT("职业限制"), "m_stCondInfo.m_iProfession" },
{ "X%d" ,TEXT("性别限制"), "m_stCondInfo.m_iSex" },
{ "Y%d" ,TEXT("等级上限"), "m_stCondInfo.m_iMaxLevel" },
{ "Z%d" ,TEXT("等级下限"), "m_stCondInfo.m_iMinLevel" },
{ "AA%d" ,TEXT("时间限制"), "m_stCondInfo.m_iTimeLimit" },
{ "AB%d" ,TEXT("组队人数下限"), "m_stCondInfo.m_iTeamMemNumMin" },
{ "AC%d" ,TEXT("组队人数上限"), "m_stCondInfo.m_iTeamMemNumMax" },
{ "AD%d" ,TEXT("接任务获取道具"), "" },
{ "AE%d" ,TEXT("完成任务获取道具"), "" },
{ "AF%d" ,TEXT("任务奖励ID"), "m_stEventInfo.m_stReward.m_astRewardId[0]" },
{ "AG%d" ,TEXT("任务奖励ID"), "m_stEventInfo.m_stReward.m_astRewardId[1]" },
{ "AH%d" ,TEXT("任务奖励ID"), "m_stEventInfo.m_stReward.m_astRewardId[2]" },
{ "AI%d" ,TEXT("任务奖励ID"), "m_stEventInfo.m_stReward.m_astRewardId[3]" },
{ "AJ%d" ,TEXT("任务奖励ID"), "m_stEventInfo.m_stReward.m_astRewardId[4]" },
{ "AK%d" ,TEXT("任务接受招怪物组"), "m_stEventInfo.m_stMonsterInfo.m_astMonsterGroupID" },
{ "AL%d" ,TEXT("任务完成招怪物组"), "" },
{ "AM%d" ,TEXT("任务接受回收怪物组"), "" },
{ "AN%d" ,TEXT("任务完成回收怪物组"), "" },
{ "AO%d" ,TEXT("异常时补招怪物组"), "" },
{ "AP%d" ,TEXT("任务目标0类型"), "m_astTargetParam[0].m_iTargetType" },
{ "AQ%d" ,TEXT("参数1"), "m_astTargetParam[0].m_astIntParam[0]" },
{ "AR%d" ,TEXT("参数2"), "m_astTargetParam[0].m_astIntParam[1]" },
{ "AS%d" ,TEXT("参数3"), "m_astTargetParam[0].m_astIntParam[2]" },
{ "AT%d" ,TEXT("参数4"), "m_astTargetParam[0].m_astIntParam[3]" },
{ "AU%d" ,TEXT("目标描述"), "m_astTargetParam[0].m_szTargetDesc" },
{ "AV%d" ,TEXT("目标变化时的文本"), "m_astTargetParam[0].m_szTargetChangeDesc" },
{ "AW%d" ,TEXT("任务目标1类型"), "m_astTargetParam[1].m_iTargetType" },
{ "AX%d" ,TEXT("参数1"), "m_astTargetParam[1].m_astIntParam[0]" },
{ "AY%d" ,TEXT("参数2"), "m_astTargetParam[1].m_astIntParam[1]" },
{ "AZ%d" ,TEXT("参数3"), "m_astTargetParam[1].m_astIntParam[2]" },
{ "BA%d" ,TEXT("参数4"), "m_astTargetParam[1].m_astIntParam[3]" },
{ "BB%d" ,TEXT("目标描述"), "m_astTargetParam[1].m_szTargetDesc" },
{ "BC%d" ,TEXT("目标变化时的文本"), "m_astTargetParam[1].m_szTargetChangeDesc" },
};

#define EXPORT_POS(ENUM,I) (FString::Printf( *EP_CELLS[(int)ENUM][0], I+3))
#define EXPORT_TGT(ENUM, J, I) (FString::Printf( *EP_CELLS[ EP_TARGETS[J][ (int)ENUM - (int)ECELL_POS::MAX ] ][0], I+3))

constexpr int EP_CELL_SIZE = sizeof(EP_CELLS) / sizeof(FString[3]);
static_assert(EP_CELL_SIZE == (int)ECELL_POS::MAX,"must equal");


struct ExportItem
{
	FString                                      ModuleName;
	TArray<class UTaskSystemExpressionSubtask_Common*> Exprs;
};

class XlsxExporter
{
public:

	static bool ExportModule(ExportItem& Item);

};