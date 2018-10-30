
/**

Copyright 2018 swd. All rights reserved.

*/

#pragma once

#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"

class IPSD2UMG : public IModuleInterface
{

public:
	static inline IPSD2UMG& Get()
	{
		return FModuleManager::LoadModuleChecked< IPSD2UMG >( "PSD2UMG" );
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded( "PSD2UMG" );
	}
};

