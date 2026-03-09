// Vehicle Wetness System. Cem Akkaya https://www.cemakkaya.com
#pragma once

#include "CoreMinimal.h"
#include "GlobalShader.h"
#include "ShaderParameterStruct.h"

class FVehicleWetnessFadeCS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FVehicleWetnessFadeCS);
	SHADER_USE_PARAMETER_STRUCT(FVehicleWetnessFadeCS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters,)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float>, OutputTexture)
		SHADER_PARAMETER(float, DeltaTime)
		SHADER_PARAMETER(float, TrailDecay)
	END_SHADER_PARAMETER_STRUCT()
};

class FVehicleWetnessCS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FVehicleWetnessCS);
	SHADER_USE_PARAMETER_STRUCT(FVehicleWetnessCS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters,)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float>, OutputTexture)
		SHADER_PARAMETER_RDG_BUFFER_UAV(RWStructuredBuffer<float4>, Droplets)
		SHADER_PARAMETER(float, DeltaTime)
		SHADER_PARAMETER(float, TotalTime)
		SHADER_PARAMETER(float, SpawnRate)
		SHADER_PARAMETER(float, MaxDropletLife)
		SHADER_PARAMETER(float, AverageDropletSpeed)
		SHADER_PARAMETER(float, SpeedRandomness)
		SHADER_PARAMETER(float, DirectionJitter)
		SHADER_PARAMETER(float, DropletStartSize)
		SHADER_PARAMETER(float, DropletEndSize)
		SHADER_PARAMETER(float, DropletSizeRandom)
		SHADER_PARAMETER(float, TrailDecay)
		SHADER_PARAMETER(FVector2f, MoveDir)
		SHADER_PARAMETER(float, VelocityInfluenceX)
		SHADER_PARAMETER(uint32, bNegateVelocityInfluenceX)
		SHADER_PARAMETER(float, VelocityInfluenceY)
		SHADER_PARAMETER(uint32, bNegateVelocityInfluenceY)
		SHADER_PARAMETER(FVector3f, VehicleVelocity)
		SHADER_PARAMETER(float, DropletDelay)
		SHADER_PARAMETER(float, DropletDelayMovementInfluence)
		SHADER_PARAMETER(float, EdgeRadiusMultiplier)
		SHADER_PARAMETER(FVector2f, WiperPivot)
		SHADER_PARAMETER(float, WiperRadius)
		SHADER_PARAMETER(float, WiperAngle)
		SHADER_PARAMETER(float, WiperPreviousAngle)
		SHADER_PARAMETER(float, WiperThickness)
		SHADER_PARAMETER(float, WiperCentrifugalStrength)
		SHADER_PARAMETER(float, WiperStickiness)
	END_SHADER_PARAMETER_STRUCT()
};

class FVehicleWetnessBlurCS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FVehicleWetnessBlurCS);
	SHADER_USE_PARAMETER_STRUCT(FVehicleWetnessBlurCS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters,)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float>, OutputTexture)
		SHADER_PARAMETER(float, BlurStrength)
	END_SHADER_PARAMETER_STRUCT()
};