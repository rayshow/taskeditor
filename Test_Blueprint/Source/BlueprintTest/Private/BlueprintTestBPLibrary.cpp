// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "BlueprintTestBPLibrary.h"
#include "BlueprintTest.h"

UBlueprintTestBPLibrary::UBlueprintTestBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

float UBlueprintTestBPLibrary::BlueprintTestSampleFunction(float Param)
{
	return -1;
}

