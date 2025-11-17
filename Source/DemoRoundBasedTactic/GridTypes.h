// GridTypes.h

#pragma once

#include "CoreMinimal.h"
#include "GridTypes.generated.h"

/**
 * Rounding policy used when converting a world position back to a grid coordinate.
 */
UENUM(BlueprintType)
enum class EGridRoundingPolicy : uint8
{
    Floor UMETA(DisplayName = "Floor"),
    Round UMETA(DisplayName = "Round"),
    Ceil  UMETA(DisplayName = "Ceil")
};

/**
 * Lightweight C++ height provider interface used by the grid geometry utilities.
 * 
 * Implementations are free to read from textures, arrays, or any other data source.
 * This is intentionally not a UObject interface to keep the core math deterministic
 * and easily testable.
 */
class IGridHeightProvider
{
public:
    virtual ~IGridHeightProvider() = default;

    /**
     * Return the world-space ground height (Z) at the given grid cell.
     * The coordinates are guaranteed to be inside [0, Width-1] × [0, Height-1]
     * if used together with a correctly configured FGridConfig.
     */
    virtual float GetHeightAt(int32 GridX, int32 GridY) const = 0;
};

/**
 * Compact configuration object that fully describes a logical grid and how it
 * is embedded into world space.
 *
 * All functions in the grid geometry library are pure with respect to this struct:
 * given the same config and input, they always return the same result.
 */
USTRUCT(BlueprintType)
struct FGridConfig
{
    GENERATED_BODY()

    /** Logical grid width (number of cells along the X axis). Valid X indices are [0, Width-1]. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
    int32 Width = 0;

    /** Logical grid height (number of cells along the Y axis). Valid Y indices are [0, Height-1]. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
    int32 Height = 0;

    /**
     * World-space location of cell (0,0), at the *centre* of the cell.
     *
     * Using the centre avoids half-cell offsets in most gameplay code.
     * If you prefer another convention (e.g. lower-left corner), shift this origin accordingly.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
    FVector GridOrigin = FVector::ZeroVector;

    /**
     * If true, the grid orientation is derived from GridRotation.
     * If false, AxisX / AxisY are used directly as basis vectors.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid|Orientation")
    bool bUseRotation = false;

    /**
     * Rotation used to derive the grid basis when bUseRotation is true.
     * X axis of the rotation corresponds to the grid X direction, and
     * Y axis corresponds to grid Y direction, both lying in the horizontal plane.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid|Orientation", meta = (EditCondition = "bUseRotation"))
    FRotator GridRotation = FRotator::ZeroRotator;

    /**
     * Explicit basis vector for the grid X axis (in world space).
     * Must be horizontal and normalized for best results.
     * Used only when bUseRotation is false.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid|Orientation", meta = (EditCondition = "!bUseRotation"))
    FVector AxisX = FVector::ForwardVector;

    /**
     * Explicit basis vector for the grid Y axis (in world space).
     * Must be horizontal and normalized for best results.
     * Used only when bUseRotation is false.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid|Orientation", meta = (EditCondition = "!bUseRotation"))
    FVector AxisY = FVector::RightVector;

    /**
     * Physical size of a single cell along each grid axis (in world units).
     * For square cells, this value is used for both the X and Y directions.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
    float CellSize = 100.f;

    /**
     * Default eye height above the ground when computing "eye level" positions.
     * This is applied on top of the ground height returned by the height provider.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
    float DefaultEyeHeight = 160.f;

    /**
     * This pointer is *not* exposed to Blueprints on purpose; in purely Blueprint
     * scenarios the grid will fall back to GridOrigin.Z as the ground height.
     * C++ systems can inject a concrete implementation at runtime.
     */
    TSharedPtr<IGridHeightProvider> HeightProvider;
};
