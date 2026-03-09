// Vehicle Wetness System. Cem Akkaya https://www.cemakkaya.com

#include "VehicleWetnessCS.h"
#include "RenderGraphUtils.h"
#include "ShaderCompilerCore.h"

IMPLEMENT_GLOBAL_SHADER(
	FVehicleWetnessFadeCS,
	"/VehicleWetness/Private/VehicleWetnessCS.usf",
	"FadeCS",
	SF_Compute
);

IMPLEMENT_GLOBAL_SHADER(
	FVehicleWetnessCS,
	"/VehicleWetness/Private/VehicleWetnessCS.usf",
	"MainCS",
	SF_Compute
);

IMPLEMENT_GLOBAL_SHADER(
	FVehicleWetnessBlurCS,
	"/VehicleWetness/Private/VehicleWetnessCS.usf",
	"BlurCS",
	SF_Compute
);