#include"HeightFogVolume.h"
#include "Engine/PostProcessVolume.h"
#include "Engine/CollisionProfile.h"
#include "Components/BrushComponent.h"
#include "Components/ExponentialHeightFogComponent.h"


AHeightFogVolume::AHeightFogVolume(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Settings = CreateDefaultSubobject<UHeightFogVolumeInfo>(TEXT("HeightFogVolumeInfo0"));
	bHidden = false;
	GetBrushComponent()->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	GetBrushComponent()->bAlwaysCreatePhysicsState = true;
	GetBrushComponent()->Mobility = EComponentMobility::Movable;
	Settings->AttachToComponent(RootComponent,FAttachmentTransformRules::KeepRelativeTransform);
	BrushColor = FColor(200, 100, 100, 255);
	bColored = true;

	bEnabled = true;
	BlendRadius = 100.0f;
	BlendWeight = 1.0f;
}


#if WITH_EDITOR
void AHeightFogVolume::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
}

void AHeightFogVolume::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

bool AHeightFogVolume::CanEditChange(const UProperty* InProperty) const
{
	return Super::CanEditChange(InProperty);
}

void AHeightFogVolume::PostUnregisterAllComponents()
{
	// Route clear to super first.
	Super::PostUnregisterAllComponents();
	if (GetWorld())
	{
		GetWorld()->ExpHeightFogVolumes.Remove(this);
	}
}

void AHeightFogVolume::PostRegisterAllComponents()
{
	// Route update to super first.
	Super::PostRegisterAllComponents();

	if (GetWorld()) {
		auto& VolumeArray = GetWorld()->ExpHeightFogVolumes;
		const int32 NumVolumes = VolumeArray.Num();
		float TargetPriority = this->GetProperties().Priority;
		int32 InsertIndex = 0;
		// TODO: replace with binary search.
		for (; InsertIndex < NumVolumes; InsertIndex++)
		{
			IInterface_ExponentialHeightFogVolume* CurrentVolume = VolumeArray[InsertIndex];
			float CurrentPriority = CurrentVolume->GetProperties().Priority;

			if (TargetPriority < CurrentPriority)
			{
				break;
			}
			if (CurrentVolume == this)
			{
				return;
			}
		}
		VolumeArray.Insert(this, InsertIndex);
	}
}




#endif // WITH_EDITOR