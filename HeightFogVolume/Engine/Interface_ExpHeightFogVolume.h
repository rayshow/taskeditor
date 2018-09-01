// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Interface.h"
#include "Interface_ExpHeightFogVolume.generated.h"

class UExponentialHeightFogComponent;

struct FExponentialHeightFogProperties
{
	const UExponentialHeightFogComponent* Settings;
	float Priority;
	float BlendRadius;
	float BlendWeight;
	bool bIsEnabled;
	bool bIsUnbound;
};

/** Interface for general PostProcessVolume access **/
UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UInterface_ExponentialHeightFogVolume : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IInterface_ExponentialHeightFogVolume
{
	GENERATED_IINTERFACE_BODY()

	ENGINE_API virtual bool EncompassesPoint(FVector Point, float SphereRadius/*=0.f*/, float* OutDistanceToPoint) = 0;
	ENGINE_API virtual FExponentialHeightFogProperties GetProperties() const = 0;
};
