#pragma once

#include "GameFramework/Volume.h"
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptInterface.h"
#include "GameFramework/Volume.h"
#include "Engine/BlendableInterface.h"
#include "Engine/Scene.h"
#include "Engine/World.h"
#include "Interfaces/Interface_ExpHeightFogVolume.h"
#include "HeightFogVolumeInfo.h"
#include "Net/UnrealNetwork.h"
#include "HeightFogVolume.generated.h"


UCLASS(autoexpandcategories = HeightFogVolume, hidecategories = (Advanced, Collision, Volume, Brush, Attachment), MinimalAPI)
class AHeightFogVolume:
	public AVolume,
	public IInterface_ExponentialHeightFogVolume
{
	GENERATED_UCLASS_BODY()

	/** @todo document */
	UPROPERTY(Category = ExponentialHeightFog, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UHeightFogVolumeInfo* Settings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HeightFogVolumeSettings)
	float Priority;

	/** World space radius around the volume that is used for blending (only if not unbound).			*/
	UPROPERTY(interp, Category = HeightFogVolumeSettings, meta = (ClampMin = "0.0", UIMin = "0.0", UIMax = "6000.0"))
	float BlendRadius;

	/** 0:no effect, 1:full effect */
	UPROPERTY(interp, Category = HeightFogVolumeSettings, BlueprintReadWrite, meta = (UIMin = "0.0", UIMax = "1.0"))
	float BlendWeight;

	/** Whether this volume is enabled or not.															*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HeightFogVolumeSettings)
	uint32 bEnabled : 1;

	/** Whether this volume covers the whole world, or just the area inside its bounds.								*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HeightFogVolumeSettings, meta = (DisplayName = "Infinite Extent (Unbound)"))
	uint32 bUnbound : 1;

public:
	virtual bool EncompassesPoint(FVector Point, float SphereRadius/*=0.f*/, float* OutDistanceToPoint) { 
		return AVolume::EncompassesPoint(Point, SphereRadius, OutDistanceToPoint);
	};

	virtual FExponentialHeightFogProperties GetProperties() const {
		FExponentialHeightFogProperties Properties;
		Properties.Priority = Priority;
		Properties.BlendRadius = BlendRadius;
		Properties.BlendWeight = BlendWeight;
		Properties.bIsEnabled = bEnabled;
		Properties.bIsUnbound = bUnbound;
		Properties.Settings = Settings;
		return Properties;
	};

	//~ Begin AActor Interface
	virtual void PostUnregisterAllComponents(void) override;

protected:
	virtual void PostRegisterAllComponents() override;
public:
	//~ End AActor Interface
#if WITH_EDITORONLY_DATA
	//~ Begin UObject Interface
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual bool CanEditChange(const UProperty* InProperty) const override;
	virtual void Serialize(FArchive& Ar);
#endif // WITH_EDITOR
	//~ End UObject Interface


	/** replicated copy of ExponentialHeightFogComponent's bEnabled property */
	UPROPERTY(replicatedUsing = OnRep_bEnabled)
	uint32 bEffectEnabled : 1;

	/** Replication Notification Callbacks */
	UFUNCTION()
	virtual void OnRep_bEnabled() {
		Settings->SetVisibility(bEffectEnabled);
	}

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);

		DOREPLIFETIME(AHeightFogVolume, bEffectEnabled);
	}


	//Begin AActor Interface
	virtual void PostInitializeComponents() override {
		Super::PostInitializeComponents();
		
		bEffectEnabled = Settings->bVisible;
	}
	//End AActor Interface
};