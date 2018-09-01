#pragma once

#include "Components/ExponentialHeightFogComponent.h"
#include "Engine/World.h"
#include "HeightFogVolumeInfo.generated.h"


// reuse Exp Height Fog data, just a config info Classs, not involving rendering
UCLASS(ClassGroup = Rendering, collapsecategories, hidecategories = (Object, Mobility), editinlinenew, meta = (BlueprintSpawnableComponent))
class UHeightFogVolumeInfo 
	: public UExponentialHeightFogComponent
{
	GENERATED_UCLASS_BODY()

	//do nothing, avoid Add Fog to scene
	virtual void CreateRenderState_Concurrent() override { }

	//do nothing, avoid Add Fog to scene
	virtual void SendRenderTransform_Concurrent() override {}

	//do nothing, avoid remove Fog form scene
	virtual void DestroyRenderState_Concurrent() override {}

	// avoid rendering
	virtual bool ShouldCreateRenderState() const
	{
		return false;
	}
};



