// GridGeometryLibrary.h

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GridTypes.h"
#include "GridGeometryLibrary.generated.h"

/**
 * Blueprint-friendly grid geometry helpers.
 *
 * These functions implement the mapping between discrete grid coordinates
 * and continuous world-space positions using an FGridConfig.
 *
 * They are intentionally stateless: all inputs are explicit parameters.
 */
UCLASS()
class DEMOROUNDBASEDTACTIC_API UGridGeometryLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    /**
     * Convert a grid coordinate into a world-space position at ground level
     * (the centre of the cell on the grid plane).
     *
     * If a height provider is present in the config, its value is used as the Z coordinate.
     * Otherwise, GridOrigin.Z is used as a flat ground height.
     */
    UFUNCTION(BlueprintPure, Category = "Grid")
    static FVector GridToWorldGround(const FGridConfig& Config, FIntPoint GridCoord);

    /**
     * Convert a grid coordinate into a world-space position at "eye level":
     * ground height plus Config.DefaultEyeHeight.
     *
     * This is typically used for LOS, aiming, and perception queries.
     */
    UFUNCTION(BlueprintPure, Category = "Grid")
    static FVector GridToWorldEye(const FGridConfig& Config, FIntPoint GridCoord);

    /**
     * Convert a world-space position back into a logical grid coordinate.
     *
     * @param Config        Grid configuration.
     * @param WorldPosition World-space position to project onto the grid plane.
     * @param OutGrid       Output grid coordinate.
     * @param bClampToBounds If true, indices are clamped into [0,Width-1] × [0,Height-1].
     *                       If false and the position lies outside the grid, the function
     *                       returns false and OutGrid is set to (-1,-1).
     * @param Rounding      Rounding policy used when mapping from continuous
     *                      coordinates to integer indices.
     *
     * @return True if OutGrid lies inside the valid grid range (or has been clamped there),
     *         false if the position was outside the grid and bClampToBounds was false.
     */
    UFUNCTION(BlueprintPure, Category = "Grid")
    static bool WorldToGrid(
        const FGridConfig& Config,
        const FVector& WorldPosition,
        FIntPoint& OutGrid,
        bool bClampToBounds,
        EGridRoundingPolicy Rounding = EGridRoundingPolicy::Floor
    );
};
