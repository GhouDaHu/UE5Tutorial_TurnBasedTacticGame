// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SmoothingBlueprintLibrary.generated.h"

/**
 * stateless BP functions to expose exponential smoothing (half‑life parameter) for SpringArm TargetArmLength / Camera FOV.
 */
UCLASS()
class DEMOROUNDBASEDTACTIC_API USmoothingBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Stateless: returns smoothed value of Current toward Target using half‑life T
	UFUNCTION(BlueprintPure, Category = "Smoothing|HalfLife", meta = (DisplayName = "Exp Smoothing (Half-Life)"))
	static float ExpSmoothingHL(float Current, float Target, float HalfLife, float DeltaTime);


	// Convenience: smooth and set SpringArm->TargetArmLength, returns the new value
	UFUNCTION(BlueprintCallable, Category = "Smoothing|HalfLife")
	static float SmoothSpringArmLength(class USpringArmComponent* SpringArm, float TargetLength, float HalfLife, float DeltaTime);


	// Convenience: smooth and set Camera->FieldOfView, returns the new value
	UFUNCTION(BlueprintCallable, Category = "Smoothing|HalfLife")
	static float SmoothCameraFOV(class UCameraComponent* Camera, float TargetFOV, float HalfLife, float DeltaTime);
};
