#include"TaskSystemExpressionInput.h"
#include"TaskSystemExpression.h"
#include"TaskSystemExpressionOutput.h"
#include"CoreObjectVersion.h"
#include"FrameworkObjectVersion.h"


void FTaskSystemExpressionInput::Connect(int32 InOutputIndex, UTaskSystemExpression* InExpression)
{
	OutputIndex = InOutputIndex;
	Expression = InExpression;

	TArray<FTaskSystemExpressionOutput> Outputs;
	Outputs = Expression->GetOutputs();
	FTaskSystemExpressionOutput* Output = &Outputs[OutputIndex];
	Mask = Output->Mask;
	MaskR = Output->MaskR;
	MaskG = Output->MaskG;
	MaskB = Output->MaskB;
	MaskA = Output->MaskA;
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
	Ar << Mask;
	Ar << MaskR;
	Ar << MaskG;
	Ar << MaskB;
	Ar << MaskA;

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