// GridGeometryLibrary.cpp

#include "GridGeometryLibrary.h"
#include "Math/RotationMatrix.h"

namespace
{
    /**
     * Resolve the grid basis vectors from the configuration.
     *
     * If bUseRotation is true, the basis comes from GridRotation.
     * Otherwise AxisX and AxisY are used directly (and normalized for safety).
     */
    static void ResolveGridAxes(const FGridConfig& Config, FVector& OutXAxis, FVector& OutYAxis)
    {
        if (Config.bUseRotation)
        {
            const FRotationMatrix RotMat(Config.GridRotation);
            OutXAxis = RotMat.GetUnitAxis(EAxis::X);
            OutYAxis = RotMat.GetUnitAxis(EAxis::Y);
        }
        else
        {
            OutXAxis = Config.AxisX.GetSafeNormal();
            OutYAxis = Config.AxisY.GetSafeNormal();
        }
    }

    /**
     * Helper to obtain the ground height for a given grid cell.
     *
     * If a height provider is configured and valid, it is used.
     * Otherwise, the grid origin's Z component is used as a flat height.
     */
    static float GetGroundHeight(const FGridConfig& Config, int32 GridX, int32 GridY)
    {
        if (Config.HeightProvider.IsValid())
        {
            return Config.HeightProvider->GetHeightAt(GridX, GridY);
        }

        return Config.GridOrigin.Z;
    }
}

FVector UGridGeometryLibrary::GridToWorldGround(const FGridConfig& Config, FIntPoint GridCoord)
{
    FVector XAxis;
    FVector YAxis;
    ResolveGridAxes(Config, XAxis, YAxis);

    // Offset to move from the cell origin to the cell centre.
    constexpr float HalfCell = 0.5f;

    const float LateralX = (static_cast<float>(GridCoord.X) + HalfCell) * Config.CellSize;
    const float LateralY = (static_cast<float>(GridCoord.Y) + HalfCell) * Config.CellSize;

    const FVector LateralOffset =
        XAxis * LateralX +
        YAxis * LateralY;

    FVector WorldPos = Config.GridOrigin + LateralOffset;
    WorldPos.Z = GetGroundHeight(Config, GridCoord.X, GridCoord.Y);

    return WorldPos;
}

FVector UGridGeometryLibrary::GridToWorldEye(const FGridConfig& Config, FIntPoint GridCoord)
{
    FVector Result = GridToWorldGround(Config, GridCoord);
    Result.Z += Config.DefaultEyeHeight;
    return Result;
}

bool UGridGeometryLibrary::WorldToGrid(
    const FGridConfig& Config,
    const FVector& WorldPosition,
    FIntPoint& OutGrid,
    bool bClampToBounds,
    EGridRoundingPolicy Rounding
)
{
    // Early-out if the grid has no area.
    if (Config.Width <= 0 || Config.Height <= 0 || Config.CellSize <= KINDA_SMALL_NUMBER)
    {
        OutGrid = FIntPoint(-1, -1);
        return false;
    }

    FVector XAxis;
    FVector YAxis;
    ResolveGridAxes(Config, XAxis, YAxis);

    // Translate into grid-local space (relative to GridOrigin).
    const FVector Local = WorldPosition - Config.GridOrigin;

    // Project onto the grid axes (we ignore the vertical component).
    const float U = FVector::DotProduct(Local, XAxis);
    const float V = FVector::DotProduct(Local, YAxis);

    // Continuous grid coordinates in cell units.
    const float Fx = U / Config.CellSize;
    const float Fy = V / Config.CellSize;

    int32 Gx = 0;
    int32 Gy = 0;

    switch (Rounding)
    {
    case EGridRoundingPolicy::Round:
        Gx = FMath::RoundToInt(Fx);
        Gy = FMath::RoundToInt(Fy);
        break;

    case EGridRoundingPolicy::Ceil:
        Gx = FMath::CeilToInt(Fx);
        Gy = FMath::CeilToInt(Fy);
        break;

    case EGridRoundingPolicy::Floor:
    default:
        Gx = FMath::FloorToInt(Fx);
        Gy = FMath::FloorToInt(Fy);
        break;
    }

    if (bClampToBounds)
    {
        Gx = FMath::Clamp(Gx, 0, Config.Width - 1);
        Gy = FMath::Clamp(Gy, 0, Config.Height - 1);

        OutGrid = FIntPoint(Gx, Gy);
        return true;
    }

    // Out-of-bounds: indicate failure and mark the output as invalid.
    if (Gx < 0 || Gx >= Config.Width || Gy < 0 || Gy >= Config.Height)
    {
        OutGrid = FIntPoint(-1, -1);
        return false;
    }

    OutGrid = FIntPoint(Gx, Gy);
    return true;
}
