// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RotateHelperLibrary.generated.h"

/**
 * Stateless helpers for turning a 2D move input (X=Right, Y=Forward)
 * into a world-space, strictly-horizontal direction using a yaw-only basis.
 * Intended for Enhanced Input (Vector2D) �� CharacterMovement or custom movement.
 */
UCLASS()
class URotateHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/** Returns a rotator that preserves only Yaw (Pitch=0, Roll=0).
	 *  Use it to keep ground movement strictly horizontal regardless of camera pitch/roll. */
	UFUNCTION(BlueprintPure, Category = "Movement|Rotate")
	static FRotator MakeYawOnly(const FRotator& R);

	/** Combines a 2D input (X=Right, Y=Forward) with a yaw-only basis to produce a world-space direction.
	 *  The result lies on the X-Y plane (Z=0 when basis has no roll) and is suitable for AddMovementInput
	 *  or manual displacement after normalization/scaling.
	 *
	 *  Typical usage in Blueprint (camera-relative movement):
	 *    - GetControlRotation �� MakeYawOnly �� GetForwardVector / GetRightVector (implicit here)
	 *    - ComputeMoveDir_FromRotator(BasisRotator, InputValueVector2D)
	 *    - Character: AddMovementInput(WorldDir, 1.0)
	 *    - Custom: Normalize(WorldDir) * Speed * DeltaTime �� AddActorWorldOffset
	 */
	UFUNCTION(BlueprintPure, Category = "Movement|Rotate")
	static FVector ComputeMoveDir_FromRotator(const FRotator& BasisRotator, const FVector2D& InputXY);
};