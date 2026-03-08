#include "VehicleWetnessComponent.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "VehicleWetnessCS.h"
#include "RHICommandList.h"

UVehicleWetnessComponent::UVehicleWetnessComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UVehicleWetnessComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!RainRenderTarget)
	{
		RainRenderTarget = NewObject<UTextureRenderTarget2D>();
	}

	RainRenderTarget->RenderTargetFormat = RTF_R32f;
	RainRenderTarget->bCanCreateUAV = true;
	RainRenderTarget->InitAutoFormat(TextureSize, TextureSize);
	RainRenderTarget->UpdateResourceImmediate(true);

	// Capture values by value to avoid race conditions
	const uint32 LocalStride = sizeof(FVector4f);
	// Droplet pool size
	const uint32 BufferCount = 4096; 

	ENQUEUE_RENDER_COMMAND(CreateDropletBuffer)(
		[this, LocalStride, BufferCount](FRHICommandListImmediate& RHICmdList)
		{
			FRDGBufferDesc RDGDesc =
				FRDGBufferDesc::CreateStructuredDesc(
					LocalStride,
					BufferCount);
			RDGDesc.Usage |= EBufferUsageFlags::SourceCopy;

			DropletPooledBuffer = AllocatePooledBuffer(RDGDesc, TEXT("DropletBuffer"));

			// Clear buffer
			RHICmdList.Transition(FRHITransitionInfo(DropletPooledBuffer->GetRHI(), ERHIAccess::Unknown, ERHIAccess::UAVCompute));
			
			FRHIBufferUAVCreateInfo UAVCreateInfo(PF_Unknown);
			RHICmdList.ClearUAVFloat(DropletPooledBuffer->GetOrCreateUAV(RHICmdList, UAVCreateInfo), FVector4f(0.0f, 0.0f, 0.0f, 0.0f));
		}
	);
}

void UVehicleWetnessComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DropletPooledBuffer.SafeRelease();

	Super::EndPlay(EndPlayReason);
}

void UVehicleWetnessComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	DispatchWetnessCS();
}

void UVehicleWetnessComponent::UpdateWiperAngle(float NewAngle)
{
	WiperPreviousAngle = WiperAngle;
	WiperAngle = NewAngle;
}

void UVehicleWetnessComponent::DispatchWetnessCS()
{
	if (!RainRenderTarget || !DropletPooledBuffer.IsValid())
		return;

	FTextureRenderTargetResource* RenderTargetResource = RainRenderTarget->GameThread_GetRenderTargetResource();
	if (!RenderTargetResource)
		return;

	const float LocalDeltaTime = GetWorld()->GetDeltaSeconds();
	const float LocalTotalTime = GetWorld()->GetTimeSeconds();
	const float LocalSpawnRate = SpawnRate;
	const float LocalMaxLife = MaxDropletLife;
	const float LocalAverageSpeed = AverageDropletSpeed;
	const float LocalSpeedRandomness = SpeedRandomness;
	const float LocalDirectionJitter = DirectionJitter;
	const float LocalStartSize = DropletStartSize;
	const float LocalEndSize = DropletEndSize;
	const float LocalSizeRandom = DropletSizeRandom;
	const float LocalTrailDecay = TrailDecay;
	const FVector2f LocalMoveDir = MoveDirection;

	FVector3f LocalVelocity = FVector3f::ZeroVector;
	if (AActor* Owner = GetOwner())
	{
		// Get world velocity and convert 
		FVector WorldVelocity = Owner->GetVelocity();
		LocalVelocity = FVector3f(Owner->GetTransform().InverseTransformVector(WorldVelocity));
	}
	const float LocalVelocityInfluenceX = VelocityInfluenceX;
	const bool LocalbNegateVelocityInfluenceX = bNegateVelocityInfluenceX;
	const float LocalVelocityInfluenceY = VelocityInfluenceY;
	const bool LocalbNegateVelocityInfluenceY = bNegateVelocityInfluenceY;
	const float LocalDropletDelay = DropletDelay;
	const float LocalDropletDelayMovementInfluence = DropletDelayMovementInfluence;
	const float LocalEdgeRadiusMultiplier = EdgeRadiusMultiplier;

	const FVector2f LocalWiperPivot = WiperPivot;
	const float LocalWiperRadius = WiperRadius;
	const float LocalWiperAngle = FMath::DegreesToRadians(WiperAngle);
	const float LocalWiperPreviousAngle = FMath::DegreesToRadians(WiperPreviousAngle);
	const float LocalWiperThickness = FMath::DegreesToRadians(WiperThicknessDegrees);
	const float LocalWiperCentrifugalStrength = WiperCentrifugalStrength;
	const float LocalWiperStickiness = WiperStickiness;
	const float LocalBlurStrength = BlurStrength;

	ENQUEUE_RENDER_COMMAND(VehicleWetnessDispatch)(
		[this, RenderTargetResource, LocalDeltaTime, LocalTotalTime, LocalSpawnRate, LocalMaxLife, LocalAverageSpeed, LocalSpeedRandomness, LocalDirectionJitter, LocalStartSize, LocalEndSize, LocalSizeRandom, LocalTrailDecay, LocalMoveDir, LocalVelocity, LocalVelocityInfluenceX, LocalbNegateVelocityInfluenceX, LocalVelocityInfluenceY, LocalbNegateVelocityInfluenceY, LocalDropletDelay, LocalDropletDelayMovementInfluence, LocalEdgeRadiusMultiplier, LocalWiperPivot, LocalWiperRadius, LocalWiperAngle, LocalWiperPreviousAngle, LocalWiperThickness, LocalWiperCentrifugalStrength, LocalWiperStickiness, LocalBlurStrength]
		(FRHICommandListImmediate& RHICmdList)
		{
			FRDGBuilder GraphBuilder(RHICmdList);

			FRDGTextureRef RDGTexture =
				GraphBuilder.RegisterExternalTexture(
					CreateRenderTarget(
						RenderTargetResource->GetRenderTargetTexture(),
						TEXT("WetnessRT"))
				);

			FRDGTextureUAVDesc OutputUAVDesc(RDGTexture);
			FRDGTextureUAVRef OutputUAV =
				GraphBuilder.CreateUAV(OutputUAVDesc);

			if (!DropletPooledBuffer.IsValid() || !DropletPooledBuffer->GetRHI())
			{
				GraphBuilder.Execute();
				return;
			}

			FRDGBufferRef RDGBuffer =
				GraphBuilder.RegisterExternalBuffer(
					DropletPooledBuffer,
					TEXT("DropletBuffer"));

			// Dispatch enough groups to cover the fixed buffer size 
			const uint32 ThreadCount = 64;
			const uint32 BufferCount = 4096; 
			FIntVector GroupCount(BufferCount / ThreadCount, 1, 1);

			if (GroupCount.X == 0)
				return;

			FVehicleWetnessFadeCS::FParameters* FadeParams =
				GraphBuilder.AllocParameters<FVehicleWetnessFadeCS::FParameters>();
			FadeParams->OutputTexture = OutputUAV;
			FadeParams->DeltaTime = LocalDeltaTime;
			FadeParams->TrailDecay = LocalTrailDecay;

			TShaderMapRef<FVehicleWetnessFadeCS> FadeCS(
				GetGlobalShaderMap(GMaxRHIFeatureLevel));

			FIntVector FadeGroupCount(
				FMath::DivideAndRoundUp((int32)RenderTargetResource->GetSizeX(), 8),
				FMath::DivideAndRoundUp((int32)RenderTargetResource->GetSizeY(), 8),
				1);

			FComputeShaderUtils::AddPass(
				GraphBuilder,
				RDG_EVENT_NAME("VehicleWetnessFadeCS"),
				FadeCS,
				FadeParams,
				FadeGroupCount
			);

			FVehicleWetnessCS::FParameters* Params =
				GraphBuilder.AllocParameters<FVehicleWetnessCS::FParameters>();

			Params->OutputTexture = OutputUAV;
			Params->Droplets = GraphBuilder.CreateUAV(FRDGBufferUAVDesc(RDGBuffer));
			Params->DeltaTime = LocalDeltaTime;
			Params->TotalTime = LocalTotalTime;
			Params->SpawnRate = LocalSpawnRate;
			Params->MaxDropletLife = LocalMaxLife;
			Params->AverageDropletSpeed = LocalAverageSpeed;
			Params->SpeedRandomness = LocalSpeedRandomness;
			Params->DirectionJitter = LocalDirectionJitter;
			Params->DropletStartSize = LocalStartSize;
			Params->DropletEndSize = LocalEndSize;
			Params->DropletSizeRandom = LocalSizeRandom;
			Params->TrailDecay = LocalTrailDecay;
			Params->MoveDir = LocalMoveDir;
			Params->VelocityInfluenceX = LocalVelocityInfluenceX;
			Params->bNegateVelocityInfluenceX = LocalbNegateVelocityInfluenceX ? 1 : 0;
			Params->VelocityInfluenceY = LocalVelocityInfluenceY;
			Params->bNegateVelocityInfluenceY = LocalbNegateVelocityInfluenceY ? 1 : 0;
			Params->VehicleVelocity = LocalVelocity;
			Params->DropletDelay = LocalDropletDelay;
			Params->DropletDelayMovementInfluence = LocalDropletDelayMovementInfluence;
			Params->EdgeRadiusMultiplier = LocalEdgeRadiusMultiplier;
			Params->WiperPivot = LocalWiperPivot;
			Params->WiperRadius = LocalWiperRadius;
			Params->WiperAngle = LocalWiperAngle;
			Params->WiperPreviousAngle = LocalWiperPreviousAngle;
			Params->WiperThickness = LocalWiperThickness;
			Params->WiperCentrifugalStrength = LocalWiperCentrifugalStrength;
			Params->WiperStickiness = LocalWiperStickiness;

			TShaderMapRef<FVehicleWetnessCS> CS(
				GetGlobalShaderMap(GMaxRHIFeatureLevel));

			FComputeShaderUtils::AddPass(
				GraphBuilder,
				RDG_EVENT_NAME("VehicleWetnessCS"),
				CS,
				Params,
				GroupCount
			);

			if (LocalBlurStrength > 0.0f)
			{
				FVehicleWetnessBlurCS::FParameters* BlurParams =
					GraphBuilder.AllocParameters<FVehicleWetnessBlurCS::FParameters>();
				BlurParams->OutputTexture = OutputUAV;
				BlurParams->BlurStrength = LocalBlurStrength;

				TShaderMapRef<FVehicleWetnessBlurCS> BlurCS(
					GetGlobalShaderMap(GMaxRHIFeatureLevel));

				FIntVector BlurGroupCount(
					FMath::DivideAndRoundUp((int32)RenderTargetResource->GetSizeX(), 8),
					FMath::DivideAndRoundUp((int32)RenderTargetResource->GetSizeY(), 8),
					1);

				FComputeShaderUtils::AddPass(
					GraphBuilder,
					RDG_EVENT_NAME("VehicleWetnessBlurCS"),
					BlurCS,
					BlurParams,
					BlurGroupCount
				);
			}

			GraphBuilder.Execute();
		}
	);
}