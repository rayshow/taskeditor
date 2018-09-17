#pragma once

#include"ObjectMacros.h"
#include"CoreMinimal.h"
#include"Object.h"
#include "LuaFileRefObject.generated.h"
//https://terraformpro.com/
UCLASS()
class ULuaFileRefObject : public UObject
{
public:
	GENERATED_UCLASS_BODY()

	UPROPERTY()
	FString RefPath;
};