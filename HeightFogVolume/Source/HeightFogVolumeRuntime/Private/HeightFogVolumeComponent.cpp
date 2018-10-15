#include"HeightFogVolumeComponent.h"

UHeightFogVolumeComponent::UHeightFogVolumeComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bEnabled = true;
	BlendRadius = 100.0f;
	BlendWeight = 1.0f;
	Priority = 0;
	bUnbound = 1;
	//Settings = CreateDefaultSubobject<UHeightFogVolumeInfo>(TEXT("HeightFogVolumeInfo0"));
}

void UHeightFogVolumeComponent::OnRegister() 
{
	Super::OnRegister();
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
void UHeightFogVolumeComponent::OnUnregister()
{
	Super::OnUnregister();
	// Route clear to super first.
	if (GetWorld())
	{
		GetWorld()->ExpHeightFogVolumes.Remove(this);
	}
}


void UHeightFogVolumeComponent::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
}

bool UHeightFogVolumeComponent::EncompassesPoint(FVector Point, float SphereRadius/*=0.f*/, float* OutDistanceToPoint)
{
	UShapeComponent* ParentShape = Cast<UShapeComponent>(GetAttachParent());
	if (ParentShape != nullptr)
	{
#if WITH_PHYSX
		FVector ClosestPoint;
		float Distance = ParentShape->GetDistanceToCollision(Point, ClosestPoint);
#else
		FBoxSphereBounds SphereBounds = ParentShape->CalcBounds(ParentShape->GetComponentTransform());
		float Distance = 0;
		if (ParentShape->IsA<USphereComponent>())
		{
			const FSphere& Sphere = SphereBounds.GetSphere();
			const FVector& Dist = Sphere.Center - Point;
			Distance = FMath::Max(0.0f, Dist.Size() - Sphere.W);
		}
		else // UBox or UCapsule shape (approx).
		{
			Distance = FMath::Sqrt(SphereBounds.GetBox().ComputeSquaredDistanceToPoint(Point));
		}
#endif

		if (OutDistanceToPoint)
		{
			*OutDistanceToPoint = Distance;
		}

		return Distance >= 0.f && Distance <= SphereRadius;
	}
	if (OutDistanceToPoint != nullptr)
	{
		*OutDistanceToPoint = 0;
	}
	return true;
}