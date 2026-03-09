// Vehicle Wetness System. Cem Akkaya https://www.cemakkaya.com

#pragma once

#include "Modules/ModuleManager.h"

class FVehicleWetnessModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
