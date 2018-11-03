
/**

Copyright 2018 swd. All rights reserved.

*/

#pragma once

#include "Runtime/CoreUObject/Public/UObject/NoExportTypes.h"
#include "Runtime/Engine/Classes/EditorFramework/AssetImportData.h"

#include "PSD2UMGCache.generated.h"

UCLASS()
class PSD2UMG_API UPSD2UMGCache : public UObject
{
	GENERATED_BODY()
public:

#if WITH_EDITORONLY_DATA
	UPROPERTY(Category = ImportSettings, VisibleAnywhere)
	UAssetImportData* AssetImportData = nullptr;
#endif

};