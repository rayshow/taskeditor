
/**

Copyright 2018 swd. All rights reserved.

*/


#include "AssetTypeActions_PSD2UMGCache.h"
//#include "PSD2UMGPrivatePCH.h"

#include "PSD2UMGCache.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

UClass* FAssetTypeActions_PSD2UMGCache::GetSupportedClass() const
{
	return UPSD2UMGCache::StaticClass();
}

void FAssetTypeActions_PSD2UMGCache::GetActions(
	const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder)
{
}

#undef LOCTEXT_NAMESPACE