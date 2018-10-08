// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "HeightFogVolumeRuntimeModule.h"
#include "HeightFogVolume.h"

#define LOCTEXT_NAMESPACE "FHeightFogVolumePluginModule"
 
void FHeightFogVolumeRuntimeModule::StartupModule()
{ 
}

void FHeightFogVolumeRuntimeModule::ShutdownModule()
{
	
}

IMPLEMENT_MODULE(FHeightFogVolumeRuntimeModule, HeightFogVolumeRuntime)

#undef LOCTEXT_NAMESPACE
	
