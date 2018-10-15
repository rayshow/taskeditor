// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptInterface.h"
#include "Components/SceneComponent.h"
#include "Engine/BlendableInterface.h"
#include "Engine/Scene.h"
#include "Components/ShapeComponent.h"
#include "Interfaces/Interface_ExpHeightFogVolume.h"
#include "Components/HeightFogVolumeInfo.h"
#include "HeightFogVolumeComponent.generated.h"

/**
*  PostProcessComponent. Enables Post process controls for blueprints.
*	Will use a parent UShapeComponent to provide volume data if available.
*/

UCLASS(ClassGroup = Rendering, collapsecategories, hidecategories = (Object), editinlinenew, meta = (BlueprintSpawnableComponent), MinimalAPI)
class UHeightFogVolumeComponent 
	: public UHeightFogVolumeInfo,
	public IInterface_ExponentialHeightFogVolume
{
	GENERATED_UCLASS_BODY()

		///** Post process settings to use for this volume. */
		//UPROPERTY(interp, Category = HeightFogVolume, meta = (ShowOnlyInnerProperties))
		//class UHeightFogVolumeInfo* Settings;

	/**
	* Priority of this volume. In the case of overlapping volumes the one with the highest priority
	* overrides the lower priority ones. The order is undefined if two or more overlapping volumes have the same priority.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HeightFogVolume)
		float Priority;

	/** World space radius around the volume that is used for blending (only if not unbound).			*/
	UPROPERTY(interp, Category = HeightFogVolume, meta = (ClampMin = "0.0", UIMin = "0.0", UIMax = "6000.0"))
		float BlendRadius;

	/** 0:no effect, 1:full effect */
	UPROPERTY(interp, Category = HeightFogVolume, BlueprintReadWrite, meta = (UIMin = "0.0", UIMax = "1.0"))
		float BlendWeight;

	/** Whether this volume is enabled or not.															*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HeightFogVolume)
		uint32 bEnabled : 1;

	/** set this to false to use the parent shape component as volume bounds. True affects the whole world regardless.		*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HeightFogVolume)
		uint32 bUnbound : 1;

	//~ Begin IInterface_PostProcessVolume Interface
	virtual bool EncompassesPoint(FVector Point, float SphereRadius/*=0.f*/, float* OutDistanceToPoint) override;
	virtual FExponentialHeightFogProperties GetProperties() const override
	{
		FExponentialHeightFogProperties Properties;
		Properties.Priority = Priority;
		Properties.BlendRadius = BlendRadius;
		Properties.BlendWeight = BlendWeight;
		Properties.bIsEnabled = bEnabled;
		Properties.bIsUnbound = bUnbound;
		Properties.Settings = this;// Settings;
		return Properties;
	}
	//~ End IInterface_PostProcessVolume Interface

	/** Adds an Blendable (implements IBlendableInterface) to the array of Blendables (if it doesn't exist) and update the weight */
	UFUNCTION(BlueprintCallable, Category = "Rendering")
	void AddOrUpdateBlendable(TScriptInterface<IBlendableInterface> InBlendableObject, float InWeight = 1.0f) {
		
	}

protected:

	virtual void OnRegister() override;
	virtual void OnUnregister() override;

	//~ Begin UObject Interface
	virtual void Serialize(FArchive& Ar) override;
	//~ End UObject Interface
};



