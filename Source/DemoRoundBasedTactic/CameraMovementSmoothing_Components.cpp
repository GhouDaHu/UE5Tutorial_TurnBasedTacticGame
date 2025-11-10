// Fill out your copyright notice in the Description page of Project Settings.



#include "CameraMovementSmoothing_Components.h"


/** Helper: compute the decay coefficient for the given half-life and step. */
static FORCEINLINE float HalfLifeDecayK(float HalfLife, float Dt)
{
	if (HalfLife <= 0.f) return 0.f; // snap to target in one step
	return FMath::Pow(0.5f, Dt / HalfLife);
}

float UCameraMovement_ExpHalfLifeComponent::Update1D(float DeltaTime)
{
	const float K = HalfLifeDecayK(HalfLife, DeltaTime);
	Current1D = Desire1D + (Current1D - Desire1D) * K;
	return Current1D;
}

FVector2D UCameraMovement_ExpHalfLifeComponent::Update2D(float DeltaTime)
{
	const float K = HalfLifeDecayK(HalfLife, DeltaTime);
	Current2D = Desire2D + (Current2D - Desire2D) * K;
	return Current2D;
}

/** Advance a critically damped spring (1D) for one step. The target is treated as constant during the step. */
static FORCEINLINE void SpringUpdate1D(float HalfLife, float Dt, float Desire, float& Current, float& Velocity)
{
	if (HalfLife <= 0.f)
	{
		Current = Desire; Velocity = 0.f; return;
	}
	const float W = FMath::Loge(2.f) / HalfLife;
	const float E = FMath::Exp(-W * Dt);
	float y = Current - Desire;
	const float j = Velocity + W * y;
	y = (y + j * Dt) * E;
	Velocity = (Velocity - W * j * Dt) * E;
	Current = Desire + y;
}

/** Advance a critically damped spring (2D) for one step. Each component is updated independently. */
static FORCEINLINE void SpringUpdate2D(float HalfLife, float Dt, const FVector2D& Desire, FVector2D& Current, FVector2D& Velocity)
{
	if (HalfLife <= 0.f)
	{
		Current = Desire; Velocity = FVector2D::ZeroVector; return;
	}
	const float W = FMath::Loge(2.f) / HalfLife;
	const float E = FMath::Exp(-W * Dt);
	FVector2D y = Current - Desire;
	const FVector2D j = Velocity + y * W;
	y = (y + j * Dt) * E;
	Velocity = (Velocity - j * (W * Dt)) * E;
	Current = Desire + y;
}

float UCameraMovement_SpringHalfLifeComponent::Update1D(float DeltaTime)
{
	SpringUpdate1D(HalfLife, DeltaTime, Desire1D, Current1D, Velocity1D);
	return Current1D;
}

FVector2D UCameraMovement_SpringHalfLifeComponent::Update2D(float DeltaTime)
{
	SpringUpdate2D(HalfLife, DeltaTime, Desire2D, Current2D, Velocity2D);
	return Current2D;
}
