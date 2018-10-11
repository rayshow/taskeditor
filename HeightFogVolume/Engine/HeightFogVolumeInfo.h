#pragma once

#include "Components/ExponentialHeightFogComponent.h"
#include "Engine/World.h"
#include "HeightFogVolumeInfo.generated.h"


// reuse Exp Height Fog data, just a config info Classs, not involving rendering
UCLASS(ClassGroup = Rendering, collapsecategories, hidecategories = (Object, Mobility), editinlinenew, meta = (BlueprintSpawnableComponent))
class ENGINE_API UHeightFogVolumeInfo
	: public UExponentialHeightFogComponent
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Overrides, meta = (PinHiddenByDefault, InlineEditConditionToggle))
	uint32 bOverride_LightColor : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Overrides, meta = (PinHiddenByDefault, InlineEditConditionToggle))
	uint32 bOverride_LightIntensity : 1;

	/** Override Direction-Light Color. */
	UPROPERTY(interp, BlueprintReadWrite, Category = "Directional Light Override", meta = (editcondition = "bOverride_SunColor", DisplayName = "Sun Color", LegacyTonemapper))
	FLinearColor LightColor;

	/** Override Direction-Light Intensity. */
	UPROPERTY(interp, BlueprintReadWrite, Category = "Directional Light Override", meta = (editcondition = "bOverride_SunIntensity", DisplayName = "Sun Intensity"))
	float LightIntensity;

	UPROPERTY(interp, BlueprintReadWrite, Category = "Directional Light Override", meta = (editcondition = "bOverride_SunIntensity", DisplayName = "Sun Intensity"))
	FQuat LightRotate;



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



