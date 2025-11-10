// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CameraMovementSmoothing_Components.generated.h"

/**
 * Exponential half-life smoothing for camera movement.
 * Holds its own state (Desire/Current) and advances only when Update* is called.
 * Unbounded movement: no min/max clamping by design.
 * Provides both 1D and 2D variants in a single component.
 */
UCLASS(ClassGroup = (Camera), BlueprintType, meta = (BlueprintSpawnableComponent))
class DEMOROUNDBASEDTACTIC_API UCameraMovement_ExpHalfLifeComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UCameraMovement_ExpHalfLifeComponent()
	{
		// This component does not auto-tick. The caller drives time via Update*.
		PrimaryComponentTick.bCanEverTick = false;
	}

	/** Response speed in seconds: the error halves every HalfLife seconds. Smaller is snappier. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params")
	float HalfLife = 0.15f;

	/** Target position/value the camera should converge to (1D). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State|1D")
	float Desire1D = 0.f;

	/** Current filtered position/value (1D). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State|1D")
	float Current1D = 0.f;

	/** Target position/value the camera should converge to (2D). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State|2D")
	FVector2D Desire2D = FVector2D::ZeroVector;

	/** Current filtered position/value (2D). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State|2D")
	FVector2D Current2D = FVector2D::ZeroVector;

	/**
	 * Advance the 1D state by DeltaTime and return the new Current.
	 * Use this per-frame (or on demand) to update towards Desire1D.
	 */
	UFUNCTION(BlueprintCallable, Category = "CameraMovement|ExpHalfLife|1D", meta = (DisplayName = "Update", CompactNodeTitle = "Update"))
	float Update1D(float DeltaTime);

	/**
	 * Advance the 2D state by DeltaTime and return the new Current vector.
	 * Use this per-frame (or on demand) to update towards Desire2D.
	 */
	UFUNCTION(BlueprintCallable, Category = "CameraMovement|ExpHalfLife|2D", meta = (DisplayName = "Update2D", CompactNodeTitle = "Update"))
	FVector2D Update2D(float DeltaTime);

	/** Instantly set Current1D to Desire1D (no smoothing). */
	UFUNCTION(BlueprintCallable, Category = "CameraMovement|ExpHalfLife|Utilities")
	void Snap1D() { Current1D = Desire1D; }

	/** Instantly set Current2D to Desire2D (no smoothing). */
	UFUNCTION(BlueprintCallable, Category = "CameraMovement|ExpHalfLife|Utilities")
	void Snap2D() { Current2D = Desire2D; }
};

/**
 * Critically damped spring smoothing expressed with a half-life parameter.
 * Holds its own state (Desire/Current/Velocity) and advances only when Update* is called.
 * Unbounded movement: no min/max clamping by design.
 * Provides both 1D and 2D variants in a single component.
 */
UCLASS(ClassGroup = (Camera), BlueprintType, meta = (BlueprintSpawnableComponent))
class DEMOROUNDBASEDTACTIC_API UCameraMovement_SpringHalfLifeComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UCameraMovement_SpringHalfLifeComponent()
	{
		// This component does not auto-tick. The caller drives time via Update*.
		PrimaryComponentTick.bCanEverTick = false;
	}

	/** Temporal response via half-life (seconds). Smaller values track the target faster without overshoot. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Params")
	float HalfLife = 0.25f;

	/** Target value (1D). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State|1D")
	float Desire1D = 0.f;

	/** Current value (1D). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State|1D")
	float Current1D = 0.f;

	/** Internal velocity (1D). Exposed for debug/telemetry/visualization. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|1D")
	float Velocity1D = 0.f;

	/** Target value (2D). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State|2D")
	FVector2D Desire2D = FVector2D::ZeroVector;

	/** Current value (2D). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State|2D")
	FVector2D Current2D = FVector2D::ZeroVector;

	/** Internal velocity (2D). Exposed for debug/telemetry/visualization. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|2D")
	FVector2D Velocity2D = FVector2D::ZeroVector;

	/** Advance the 1D spring state by DeltaTime and return the new Current. */
	UFUNCTION(BlueprintCallable, Category = "CameraMovement|SpringHalfLife|1D", meta = (DisplayName = "Update", CompactNodeTitle = "Update"))
	float Update1D(float DeltaTime);

	/** Advance the 2D spring state by DeltaTime and return the new Current vector. */
	UFUNCTION(BlueprintCallable, Category = "CameraMovement|SpringHalfLife|2D", meta = (DisplayName = "Update2D", CompactNodeTitle = "Update"))
	FVector2D Update2D(float DeltaTime);

	/** Instantly set Current1D to Desire1D and reset velocity. */
	UFUNCTION(BlueprintCallable, Category = "CameraMovement|SpringHalfLife|Utilities")
	void Snap1D() { Current1D = Desire1D; Velocity1D = 0.f; }

	/** Instantly set Current2D to Desire2D and reset velocity. */
	UFUNCTION(BlueprintCallable, Category = "CameraMovement|SpringHalfLife|Utilities")
	void Snap2D() { Current2D = Desire2D; Velocity2D = FVector2D::ZeroVector; }
};