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
{ TEXT("A%d") ,TEXT("任务ID"),   TEXT("m_stBaseInfo.m_iTaskID") },
{ TEXT("B%d") ,TEXT("任务类型"), TEXT("m_stBaseInfo.m_iTaskType") },
{ TEXT("C%d") ,TEXT("任务名字"), TEXT("m_stBaseInfo.m_szTaskName") },
{ TEXT("D%d") ,TEXT("任务描述"), TEXT("m_stBaseInfo.m_szTaskDesc") },
{ TEXT("E%d") ,TEXT("接受任务描述"), TEXT("m_stBaseInfo.m_szTaskDescAccept") },
{ TEXT("F%d"),TEXT("任务进行中描述"), TEXT("m_stBaseInfo.m_szTaskDescDoing") },
{ TEXT("G%d") ,TEXT("任务完成描述"), TEXT("m_stBaseInfo.m_szTaskDescFinish") },
{ TEXT("H%d") ,TEXT("起始NPC"), TEXT("m_stBaseInfo.m_iStartNpc") },
{ TEXT("I%d") ,TEXT("起始NPC对话ID"), TEXT("m_stBaseInfo.m_iAcceptDialogID") },
{ TEXT("J%d") ,TEXT("完成NPC"), TEXT("m_stBaseInfo.m_iFinishNpc") },
{ TEXT("K%d") ,TEXT("完成NPC对话ID"), TEXT("m_stBaseInfo.m_iFinishDialogID") },
{ TEXT("L%d") ,TEXT("前置任务关系"), TEXT("m_stCondInfo.m_iPreTaskRelation") },
{ TEXT("M%d") ,TEXT("前置任务ID"), TEXT("m_stCondInfo.astPreTaskID[0]") },
{ TEXT("N%d") ,TEXT("后置任务"), TEXT("m_stEventInfo.m_astNextTaskID") },
{ TEXT("O%d") ,TEXT("后置任务关系"), TEXT("m_stEventInfo.m_iNexTaskRelation") },
{ TEXT("P%d") ,TEXT("后置任务参数"), TEXT("m_stEventInfo.m_astNextTaskParam") },
{ TEXT("Q%d") ,TEXT("是否自动领取任务"), TEXT("m_stFlagInfo.m_bIsAutoAccept") },
{ TEXT("R%d") ,TEXT("任务是否可放弃"),TEXT("") },
{ TEXT("S%d") ,TEXT("失败是否自动放弃"), TEXT("m_stFlagInfo.m_bTimeoutAutoGiveUp") },
{ TEXT("T%d") ,TEXT("放弃是否可重接"), TEXT("m_stFlagInfo.m_bGiveUpAutoReAccept") },
{ TEXT("U%d") ,TEXT("是否可通过前端直接提交"), TEXT("m_stFlagInfo.m_bCanSubmitFromClient") },
{ TEXT("V%d") ,TEXT("能否自动完成"), TEXT("m_stFlagInfo.m_bIsAutoFinish") },
{ TEXT("W%d") ,TEXT("职业限制"), TEXT("m_stCondInfo.m_iProfession") },
{ TEXT("X%d") ,TEXT("性别限制"), TEXT("m_stCondInfo.m_iSex") },
{ TEXT("Y%d") ,TEXT("等级上限"), TEXT("m_stCondInfo.m_iMaxLevel") },
{ TEXT("Z%d") ,TEXT("等级下限"), TEXT("m_stCondInfo.m_iMinLevel") },
{ TEXT("AA%d") ,TEXT("时间限制"), TEXT("m_stCondInfo.m_iTimeLimit") },
{ TEXT("AB%d") ,TEXT("组队人数下限"), TEXT("m_stCondInfo.m_iTeamMemNumMin") },
{ TEXT("AC%d") ,TEXT("组队人数上限"), TEXT("m_stCondInfo.m_iTeamMemNumMax") },
{ TEXT("AD%d") ,TEXT("接任务获取道具"), TEXT("") },
{ TEXT("AE%d") ,TEXT("完成任务获取道具"), TEXT("") },
{ TEXT("AF%d") ,TEXT("任务奖励ID"), TEXT("m_stEventInfo.m_stReward.m_astRewardId[0]") },
{ TEXT("AG%d") ,TEXT("任务奖励ID"), TEXT("m_stEventInfo.m_stReward.m_astRewardId[1]") },
{ TEXT("AH%d") ,TEXT("任务奖励ID"), TEXT("m_stEventInfo.m_stReward.m_astRewardId[2]") },
{ TEXT("AI%d") ,TEXT("任务奖励ID"), TEXT("m_stEventInfo.m_stReward.m_astRewardId[3]") },
{ TEXT("AJ%d") ,TEXT("任务奖励ID"), TEXT("m_stEventInfo.m_stReward.m_astRewardId[4]") },
{ TEXT("AK%d") ,TEXT("任务接受招怪物组"), TEXT("m_stEventInfo.m_stMonsterInfo.m_astMonsterGroupID") },
{ TEXT("AL%d") ,TEXT("任务完成招怪物组"), TEXT("") },
{ TEXT("AM%d") ,TEXT("任务接受回收怪物组"), TEXT("") },
{ TEXT("AN%d") ,TEXT("任务完成回收怪物组"), TEXT("") },
{ TEXT("AO%d") ,TEXT("异常时补招怪物组"), TEXT("") },
{ TEXT("AP%d") ,TEXT("任务目标0类型"), TEXT("m_astTargetParam[0].m_iTargetType") },
{ TEXT("AQ%d") ,TEXT("参数1"), TEXT("m_astTargetParam[0].m_astIntParam[0]") },
{ TEXT("AR%d") ,TEXT("参数2"), TEXT("m_astTargetParam[0].m_astIntParam[1]") },
{ TEXT("AS%d") ,TEXT("参数3"), TEXT("m_astTargetParam[0].m_astIntParam[2]") },
{ TEXT("AT%d") ,TEXT("参数4"), TEXT("m_astTargetParam[0].m_astIntParam[3]") },
{ TEXT("AU%d") ,TEXT("目标描述"), TEXT("m_astTargetParam[0].m_szTargetDesc") },
{ TEXT("AV%d") ,TEXT("目标变化时的文本"), TEXT("m_astTargetParam[0].m_szTargetChangeDesc") },
{ TEXT("AW%d") ,TEXT("任务目标1类型"), TEXT("m_astTargetParam[1].m_iTargetType") },
{ TEXT("AX%d") ,TEXT("参数1"), TEXT("m_astTargetParam[1].m_astIntParam[0]") },
{ TEXT("AY%d") ,TEXT("参数2"), TEXT("m_astTargetParam[1].m_astIntParam[1]") },
{ TEXT("AZ%d") ,TEXT("参数3"), TEXT("m_astTargetParam[1].m_astIntParam[2]") },
{ TEXT("BA%d") ,TEXT("参数4"), TEXT("m_astTargetParam[1].m_astIntParam[3]") },
{ TEXT("BB%d") ,TEXT("目标描述"), TEXT("m_astTargetParam[1].m_szTargetDesc") },
{ TEXT("BC%d") ,TEXT("目标变化时的文本"), TEXT("m_astTargetParam[1].m_szTargetChangeDesc") },
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