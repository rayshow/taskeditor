
/**

Copyright 2018 swd. All rights reserved.

*/

#include "PSD2UMGPrivatePCH.h"
#include "AssetTypeActions_PSD2UMGCache.h"

class FPSD2UMG : public IPSD2UMG
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	TSharedPtr<FAssetTypeActions_PSD2UMGCache> PSD2UMGCache_AssetTypeActions;
};

void FPSD2UMG::StartupModule()
{
	PSD2UMGCache_AssetTypeActions = MakeShareable(new FAssetTypeActions_PSD2UMGCache);

	FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get()
		.RegisterAssetTypeActions(PSD2UMGCache_AssetTypeActions.ToSharedRef());
}


void FPSD2UMG::ShutdownModule()
{
	if (PSD2UMGCache_AssetTypeActions.IsValid())
	{
		if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
		{
			FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get()
				.UnregisterAssetTypeActions(PSD2UMGCache_AssetTypeActions.ToSharedRef());
		}
		PSD2UMGCache_AssetTypeActions.Reset();
	}
}

IMPLEMENT_MODULE(FPSD2UMG, PSD2UMG)
