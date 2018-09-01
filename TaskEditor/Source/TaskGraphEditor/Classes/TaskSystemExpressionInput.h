#pragma once

#include"CoreMinimal.h"
#include "UObject/Class.h"
#include "SceneTypes.h"
#include"ObjectMacros.h"

//@warning: FExpressionInput is mirrored in MaterialExpression.h and manually "subclassed" in Material.h (FMaterialInput)
USTRUCT(noexport)
struct FTaskSystemExpressionInput
{
#if WITH_EDITORONLY_DATA
	/**
	* Material expression that this input is connected to, or NULL if not connected.
	* If you want to be safe when checking against dangling Reroute nodes, please use GetTracedInput before accessing this property.
	*/
	UPROPERTY()
	class UTaskSystemExpression*	Expression;
#endif

	/**
	* Index into Expression's outputs array that this input is connected to.
	* If you want to be safe when checking against dangling Reroute nodes, please use GetTracedInput before accessing this property.
	*/
	UPROPERTY()
	int32						OutputIndex;

	/**
	* Optional name of the input.
	* Note that this is the only member which is not derived from the output currently connected.
	*/
	UPROPERTY()
	FName						InputName;

	UPROPERTY()
	int32						Mask;

	UPROPERTY()
	int32 MaskR;

	UPROPERTY()
	int32 MaskG;

	UPROPERTY()
	int32 MaskB;

	UPROPERTY()
	int32 MaskA;

	/** Material expression name that this input is connected to, or None if not connected. Used only in cooked builds */
	UPROPERTY()
	FName ExpressionName;
	
	FTaskSystemExpressionInput()
		: OutputIndex(0)
		, Mask(0)
		, MaskR(0)
		, MaskG(0)
		, MaskB(0)
		, MaskA(0)
	{
#if WITH_EDITORONLY_DATA
		Expression = nullptr;
#endif
	}

	/**
	* Tests if the input has a material expression connected to it
	*
	* @return	true if an expression is connected, otherwise false
	*/
	bool IsConnected() const
	{
#if WITH_EDITORONLY_DATA
		return (nullptr != Expression);
#else
		return ExpressionName != NAME_None;
#endif // WITH_EDITORONLY_DATA
	}

#if WITH_EDITOR
	/** Connects output of InExpression to this input */
	void Connect(int32 InOutputIndex, class UTaskSystemExpression* InExpression);
#endif // WITH_EDITOR

	/** ICPPStructOps interface */
	bool Serialize(FArchive& Ar);

	/** If this input goes through reroute nodes or other paths that should not affect code, trace back on the input chain.*/
	FTaskSystemExpressionInput GetTracedInput() const;

	/** Helper for setting component mask. */
	void SetMask(int32 UseMask, int32 R, int32 G, int32 B, int32 A)
	{
		Mask = UseMask;
		MaskR = R;
		MaskG = G;
		MaskB = B;
		MaskA = A;
	}
};

template<>
struct TStructOpsTypeTraits<FTaskSystemExpressionInput>
	: public TStructOpsTypeTraitsBase2<FTaskSystemExpressionInput>
{
	enum
	{
		WithSerializer = true,
	};
};
