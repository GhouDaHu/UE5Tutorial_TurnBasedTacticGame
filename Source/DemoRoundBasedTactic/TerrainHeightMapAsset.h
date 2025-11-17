#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/Texture2D.h"
#include "TerrainHeightMapAsset.generated.h"

/**
 * Data asset that stores a height map decoded from a 16‑bit grayscale texture.
 *
 * CellHeights is a flat array of world‑space height values (centimeters) for a
 * grid.  Each entry corresponds to one grid cell and is stored in row‑major
 * order.  The indexing convention used throughout this project is:
 *     Index = Y * Width + X
 * where X is the column in the range [0, Width‑1] and Y is the row in the
 * range [0, Height‑1].  Consumers of this asset should use the same
 * convention when accessing CellHeights.
 */
UCLASS(BlueprintType)
class UTerrainHeightMapAsset : public UDataAsset
{
    GENERATED_BODY()

public:
    /** Width of the height map in cells */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 Width = 0;

    /** Height of the height map in cells */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 Height = 0;

    /**
     * Flattened array of heights in world units (centimeters).
     * The array has Width * Height entries.  Use the row‑major index
     * (Y * Width + X) to access individual cells.
     */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<float> CellHeights;
};

/**
 * Blueprint function library for creating UTerrainHeightMapAsset instances from
 * 16‑bit grayscale textures.  The provided function reads a UTexture2D,
 * interprets its pixel data as normalized heights, applies a world Z scale
 * and writes the results into a new data asset at the specified package path.
 */
UCLASS()
class UTerrainHeightMapLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /**
     * Decode a 16‑bit grayscale texture into a terrain height map asset.
     *
     * @param HeightTexture  Texture imported from a PNG height map.  It is
     *                       expected to have sRGB disabled and a source format
     *                       of TSF_G16 (one 16‑bit channel per pixel).  If null
     *                       or invalid, the function logs an error and
     *                       returns nullptr.
     * @param WorldZScale    Multiplier that maps normalized [0,1] height to
     *                       world‑space units (centimeters).  The resulting
     *                       CellHeights values are scaled by this factor.
     * @return The created UTerrainHeightMapAsset, or nullptr on failure.
     */
    UFUNCTION(BlueprintCallable, CallInEditor, Category = "Terrain|HeightMap")
    static UTerrainHeightMapAsset* CreateHeightMapAssetFromTexture(
        TSoftObjectPtr<UTexture2D> HeightTexture,
        float WorldZScale
    );
};
