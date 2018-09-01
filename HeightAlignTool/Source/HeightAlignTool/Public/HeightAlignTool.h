#pragma once

#include"ModuleInterface.h"
#include"ObjectMacros.h"


class FHeightAlignTool :public IModuleInterface
{
public:

	virtual void StartupModule();

	virtual void ShutdownModule();
};