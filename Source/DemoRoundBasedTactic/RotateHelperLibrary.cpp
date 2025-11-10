// Fill out your copyright notice in the Description page of Project Settings.


#include "RotateHelperLibrary.h"
#include "Math/RotationMatrix.h"

FRotator URotateHelperLibrary::MakeYawOnly(const FRotator& R)
{
	return FRotator(0.f, R.Yaw, 0.f);
}

static FORCEINLINE FVector BasisForward(const FRotator& YawOnly)
{
	// Forward axis of the basis used as the "Forward" unit vector (X axis in UE).
	return FRotationMatrix(YawOnly).GetUnitAxis(EAxis::X);
}

static FORCEINLINE FVector BasisRight(const FRotator& YawOnly)
{
	// Right axis of the basis used as the "Right" unit vector (Y axis in UE).
	return FRotationMatrix(YawOnly).GetUnitAxis(EAxis::Y);
}

FVector URotateHelperLibrary::ComputeMoveDir_FromRotator(const FRotator& BasisRotator, const FVector2D& InputXY)
{
	// Project the 2D input onto the basis: WorldDir = Fwd * Y + Right * X.
	const FRotator YawOnly = MakeYawOnly(BasisRotator);
	const FVector  Fwd = BasisForward(YawOnly);
	const FVector  Right = BasisRight(YawOnly);
	return Fwd * InputXY.Y + Right * InputXY.X;
}