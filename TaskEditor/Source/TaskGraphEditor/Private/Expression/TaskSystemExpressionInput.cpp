#include"Expression/TaskSystemExpressionInput.h"
#include"Expression/TaskSystemExpression.h"
#include"Expression/TaskSystemExpressionOutput.h"
#include"TaskSystemGraphSchema.h"
#include"CoreObjectVersion.h"
#include"FrameworkObjectVersion.h"

FName FTaskSystemExpressionInput::InputEnumToName() {
	switch (InputType) {
	case TIT_NPCEntry:
	case TIT_NPCEntry_Test:
		return UTaskSystemGraphSchema::PC_TaskSubtarget;
	case TIT_EventInput :
		return UTaskSystemGraphSchema::PC_Event;
	case TIT_DialogEntry:
		return UTaskSystemGraphSchema::PC_Flow;
	default:
		check(false);
	}
	return NAME_None;
}

FName FTaskSystemExpressionInput::InputSubEnumToName()
{
	switch (InputType) {
	case TIT_NPCEntry_Test:
		return UTaskSystemGraphSchema::PSC_Test;
	default:
		return NAME_None;
	}
}


void FTaskSystemExpressionInput::Connect(int32 InOutputIndex, UTaskSystemExpression* InExpression)
{
	OutputIndex = InOutputIndex;
	Expression = InExpression;

	TArray<FTaskSystemExpressionOutput> Outputs;
	Outputs = Expression->GetOutputs();
	FTaskSystemExpressionOutput* Output = &Outputs[OutputIndex];
}

/** ICPPStructOps interface */
bool FTaskSystemExpressionInput::Serialize(FArchive& Ar)
{
	Ar.UsingCustomVersion(FCoreObjectVersion::GUID);
	Ar.UsingCustomVersion(FFrameworkObjectVersion::GUID);

	if (Ar.CustomVer(FCoreObjectVersion::GUID) < FCoreObjectVersion::MaterialInputNativeSerialize)
	{
		return false;
	}

//#if WITH_EDITORONLY_DATA
//	if (!Ar.IsFilterEditorOnly())
//	{
//		Ar << Expression;
//	}
//#endif
	Ar << OutputIndex;
	if (Ar.CustomVer(FFrameworkObjectVersion::GUID) >= FFrameworkObjectVersion::PinsStoreFName)
	{
		Ar << InputName;
	}
	else
	{
		FString InputNameStr;
		Ar << InputNameStr;
		InputName = *InputNameStr;
	}

	// Some expressions may have been stripped when cooking and Expression can be null after loading
	// so make sure we keep the information about the connected node in cooked packages
	if (Ar.IsFilterEditorOnly())
	{
#if WITH_EDITORONLY_DATA
		if (Ar.IsSaving())
		{
			ExpressionName = Expression ? Expression->GetFName() : NAME_None;
		}
#endif // WITH_EDITORONLY_DATA
		Ar << ExpressionName;
	}

	return true;
}


/** If this input goes through reroute nodes or other paths that should not affect code, trace back on the input chain.*/
FTaskSystemExpressionInput FTaskSystemExpressionInput::GetTracedInput() const
{
	return *this;
}