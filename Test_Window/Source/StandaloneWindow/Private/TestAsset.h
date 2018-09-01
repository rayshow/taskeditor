#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "Runtime/Engine/Classes/Materials/MaterialInterface.h"
#include "TestAsset.generated.h"

UCLASS(Blueprintable)
class UMyObject : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "My Object Properties")
	FString Name;

	UPROPERTY(EditAnywhere, Category = "My Object Properties")
	TWeakObjectPtr<UMaterialInterface> Material;
};