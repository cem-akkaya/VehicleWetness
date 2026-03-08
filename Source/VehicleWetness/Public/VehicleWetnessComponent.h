#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "RenderGraphResources.h"
#include "VehicleWetnessComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class VEHICLEWETNESS_API UVehicleWetnessComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UVehicleWetnessComponent();

	/** The render target that stores the wetness mask. */
	UPROPERTY(EditAnywhere, Category="Wetness")
	UTextureRenderTarget2D* RainRenderTarget;

	/** Resolution of the wetness texture. */
	UPROPERTY(EditAnywhere, Category="Wetness", meta=(ClampMin="128", ClampMax="4096", UIMin="128", UIMax="2048"))
	int32 TextureSize = 1024;

	/** Number of new droplets spawned per second. */
	UPROPERTY(EditAnywhere, Category="Wetness", meta=(ClampMin="0.1", UIMin="0.1", UIMax="2000.0"))
	float SpawnRate = 150.0f;

	/** Maximum lifetime of a droplet in seconds before it disappears. */
	UPROPERTY(EditAnywhere, Category="Wetness", meta=(ClampMin="0.1", UIMin="0.1", UIMax="20.0"))
	float MaxDropletLife = 10.0f;

	/** Base speed of droplets in UV units per second. */
	UPROPERTY(EditAnywhere, Category="Wetness", meta=(ClampMin="0.01", UIMin="0.01", UIMax="1.0"))
	float AverageDropletSpeed = 0.5f;

	/** Randomized variation added to the base speed (0.0 to 1.0). */
	UPROPERTY(EditAnywhere, Category="Wetness", meta=(ClampMin="0.0", UIMin="0.0", UIMax="1.0"))
	float SpeedRandomness = 0.5f;

	/** Amount of random direction variation applied to droplets (0.0 to 1.0). */
	UPROPERTY(EditAnywhere, Category="Wetness", meta=(ClampMin="0.0", UIMin="0.0", UIMax="1.0"))
	float DirectionJitter = 0.5f;

	/** Starting radius of a droplet in pixels when it is first spawned. */
	UPROPERTY(EditAnywhere, Category="Wetness", meta=(ClampMin="0.1", UIMin="0.1", UIMax="20.0"))
	float DropletStartSize = 4.0f;

	/** Ending radius of a droplet in pixels when its lifetime reaches zero. */
	UPROPERTY(EditAnywhere, Category="Wetness", meta=(ClampMin="0.1", UIMin="0.1", UIMax="20.0"))
	float DropletEndSize = 2.0f;

	/** Randomized variation applied to the droplet's start and end sizes (0.0 to 1.0). */
	UPROPERTY(EditAnywhere, Category="Wetness", meta=(ClampMin="0.0", UIMin="0.0", UIMax="1.0"))
	float DropletSizeRandom = 0.5f;

	/** Rate at which the trail behind moving droplets fades (0.0 to 1.0, where 1.0 is permanent). */
	UPROPERTY(EditAnywhere, Category="Wetness", meta=(ClampMin="0.0", UIMin="0.0", UIMax="1.0"))
	float TrailDecay = 0.75f;

	/** Global direction vector for droplet movement in texture space. */
	UPROPERTY(EditAnywhere, Category="Wetness")
	FVector2f MoveDirection = FVector2f(0.0f, 1.0f);

	/** How much the vehicle's forward velocity (Local X) affects the droplet movement (UV Y). */
	UPROPERTY(EditAnywhere, Category="Wetness", meta=(ClampMin="0.0", UIMin="0.0", UIMax="10.0"))
	float VelocityInfluenceX = 0.5f;

	/** If true, negates the influence of the vehicle's forward velocity. */
	UPROPERTY(EditAnywhere, Category="Wetness")
	bool bNegateVelocityInfluenceX = false;

	/** How much the vehicle's sideways velocity (Local Y) affects the droplet movement (UV X). */
	UPROPERTY(EditAnywhere, Category="Wetness", meta=(ClampMin="0.0", UIMin="0.0", UIMax="10.0"))
	float VelocityInfluenceY = 5.0f;

	/** If true, negates the influence of the vehicle's sideways velocity. */
	UPROPERTY(EditAnywhere, Category="Wetness")
	bool bNegateVelocityInfluenceY = true;

	/** How much radius is considered for edge accumulation. */
	UPROPERTY(EditAnywhere, Category="Wetness", meta=(ClampMin="0.0", UIMin="0.0", UIMax="10.0"))
	float EdgeRadiusMultiplier = 1.0f;

	/** Wiper pivot position in UV space. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wetness|Wiper")
	FVector2f WiperPivot = FVector2f(0.2f, 1.0f);

	/** The length of the wiper in UV space. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wetness|Wiper", meta=(ClampMin="0.0"))
	float WiperRadius = 0.8f;

	/** Current angle of the wiper in degrees. Update this from a blueprint timeline or via UpdateWiperAngle! */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wetness|Wiper")
	float WiperAngle = 0.0f;

	/** Previous angle of the wiper to determine the direction of the sweep. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Wetness|Wiper")
	float WiperPreviousAngle = 0.0f;

	/** Thickness of the wiper blade in degrees (how close droplets need to be to get pushed). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wetness|Wiper")
	float WiperThicknessDegrees = 2.0f;

	/** Strength of the centrifugal force pushing droplets outward along the wiper blade. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wetness|Wiper", meta=(ClampMin="0.0"))
	float WiperCentrifugalStrength = 1.0f;

	/** How much the droplet 'sticks' to the surface or wiper, resisting the push (0.0 = slides easily, 1.0 = highly resistant). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wetness|Wiper", meta=(ClampMin="0.0", ClampMax="1.0"))
	float WiperStickiness = 0.0f;

	/** Amount of blur applied to the wetness texture (0.0 = no blur, 1.0 = full blur). */
	UPROPERTY(EditAnywhere, Category="Wetness", meta=(ClampMin="0.0", ClampMax="1.0"))
	float BlurStrength = 0.0f;

	/** Updates the wiper angle and stores the previous one to trigger the sweep logic in GPU. */
	UFUNCTION(BlueprintCallable, Category="Wetness|Wiper")
	void UpdateWiperAngle(float NewAngle);

	/** Delay in seconds after a droplet is spawned before it starts moving and shrinking. */
	UPROPERTY(EditAnywhere, Category="Wetness", meta=(ClampMin="0.0", UIMin="0.0", UIMax="10.0"))
	float DropletDelay = 1.0f;

	/** Influence of movement (gravity + airflow) at the beginning of the DropletDelay period.
	 *  Starts at this value and linearly increases to 1.0 at the end of the delay. */
	UPROPERTY(EditAnywhere, Category="Wetness", meta=(ClampMin="0.0", UIMin="0.0", UIMax="1.0"))
	float DropletDelayMovementInfluence = 0.5f;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

	void DispatchWetnessCS();

	// Persistent GPU state
	TRefCountPtr<FRDGPooledBuffer> DropletPooledBuffer;
};