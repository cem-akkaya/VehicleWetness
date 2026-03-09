// Vehicle Wetness System. Cem Akkaya https://www.cemakkaya.com

#include "VehicleWetness.h"

#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FVehicleWetnessModule"

void FVehicleWetnessModule::StartupModule()
{
	FString ShaderDir = FPaths::Combine(
		IPluginManager::Get().FindPlugin(TEXT("VehicleWetness"))->GetBaseDir(),
		TEXT("Shaders")
	);

	AddShaderSourceDirectoryMapping(TEXT("/VehicleWetness"), ShaderDir);
}

void FVehicleWetnessModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FVehicleWetnessModule, VehicleWetness)