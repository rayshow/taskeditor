#pragma once

#include"ObjectMacros.h"
#include"CoreMinimal.h"
#include"Object.h"
#include"LuaFileRefObject.generated.h"

UCLASS()
class ULuaFileRefObject : public UObject
{
public:
	GENERATED_UCLASS_BODY()

	UPROPERTY()
	FString RefPath;
};