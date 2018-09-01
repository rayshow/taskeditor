#pragma once


#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "Runtime/Engine/Classes/Materials/MaterialInterface.h"
#include "Test1.generated.h"


UENUM()
enum class TestEnum : int8
{
	Invalid = -1,

	/** Flatten may both raise and lower values */
	Both = 0,

	/** Flatten may only raise values, values above the clicked point will be left unchanged */
	Raise = 1,

	/** Flatten may only lower values, values below the clicked point will be left unchanged */
	Lower = 2,
};


UCLASS()
class UTest : public UObject
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(Category = "Value", EditAnywhere, NonTransactional, meta = (ClampMin = "0", ClampMax = "10", UIMin = "0", UIMax = "1"))
	float FloatValue;

	UPROPERTY(Category = "Value", NonTransactional, EditAnywhere)
	bool bUseWeightTargetValue;

	UPROPERTY(Category = "Value", EditAnywhere, NonTransactional, meta = (ShowForTools = "Flatten"))
	TestEnum FlattenMode;

	UPROPERTY(Category = "Shading", EditAnywhere, meta = (DisplayName = "Material", ShowForTools = "NewLandscape"))
	TWeakObjectPtr<UMaterialInterface> Material;

	UPROPERTY(Category = "Shading", EditAnywhere)
	FTransform Trans;
};

