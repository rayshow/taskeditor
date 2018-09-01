#pragma once

#include"CoreMinimal.h"

struct FTaskSystemExpressionOutput
{
	FName	OutputName;
	int32	Mask,
		MaskR,
		MaskG,
		MaskB,
		MaskA;

	FTaskSystemExpressionOutput(int32 InMask = 0, int32 InMaskR = 0, int32 InMaskG = 0, int32 InMaskB = 0, int32 InMaskA = 0) :
		Mask(InMask),
		MaskR(InMaskR),
		MaskG(InMaskG),
		MaskB(InMaskB),
		MaskA(InMaskA)
	{}

	FTaskSystemExpressionOutput(FName InOutputName, int32 InMask = 0, int32 InMaskR = 0, int32 InMaskG = 0, int32 InMaskB = 0, int32 InMaskA = 0) :
		OutputName(InOutputName),
		Mask(InMask),
		MaskR(InMaskR),
		MaskG(InMaskG),
		MaskB(InMaskB),
		MaskA(InMaskA)
	{}

	/** Helper for setting component mask. */
	ENGINE_API void SetMask(int32 UseMask, int32 R, int32 G, int32 B, int32 A)
	{
		Mask = UseMask;
		MaskR = R;
		MaskG = G;
		MaskB = B;
		MaskA = A;
	}
};
