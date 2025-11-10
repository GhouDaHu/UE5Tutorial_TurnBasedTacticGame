// Fill out your copyright notice in the Description page of Project Settings.


#include "SmoothingBlueprintLibrary.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Math/UnrealMathUtility.h"


static FORCEINLINE float ExpSmoothHL_Internal(float Current, float Target, float HalfLife, float dt)
{
	if (HalfLife <= SMALL_NUMBER) return Target; // zero/near‑zero T → snap
	const float Lambda = 0.6931471805599453f / HalfLife; // ln(2)/T
	const float OneMinusS = 1.f - FMath::Exp(-Lambda * FMath::Max(0.f, dt));
	return Current + (Target - Current) * OneMinusS;
}


float USmoothingBlueprintLibrary::ExpSmoothingHL(float Current, float Target, float HalfLife, float DeltaTime)
{
	return ExpSmoothHL_Internal(Current, Target, HalfLife, DeltaTime);
}


float USmoothingBlueprintLibrary::SmoothSpringArmLength(USpringArmComponent* SpringArm, float TargetLength, float HalfLife, float DeltaTime)
{
	if (!SpringArm) return TargetLength;
	const float Smoothed = ExpSmoothHL_Internal(SpringArm->TargetArmLength, TargetLength, HalfLife, DeltaTime);
	SpringArm->TargetArmLength = Smoothed;
	return Smoothed;
}


float USmoothingBlueprintLibrary::SmoothCameraFOV(UCameraComponent* Camera, float TargetFOV, float HalfLife, float DeltaTime)
{
	if (!Camera) return TargetFOV;
	const float Smoothed = ExpSmoothHL_Internal(Camera->FieldOfView, TargetFOV, HalfLife, DeltaTime);
	Camera->SetFieldOfView(Smoothed);
	return Smoothed;
}