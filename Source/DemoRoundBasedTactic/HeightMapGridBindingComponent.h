#pragma once
#include "GridTypes.h"
#include "TerrainHeightMapAsset.h"
#include "HeightMapGridBindingComponent.generated.h"


/**
* Scene component that owns a logical grid configuration and binds it to a height map asset.
*
* Designers select a UTerrainHeightMapAsset in the Details panel, along with the grid
* embedding parameters (origin, orientation, cell size). When the component is registered,
* it builds a runtime FGridConfig and injects an IGridHeightProvider implementation backed
* by the asset's CellHeights array.
*
* This is a direct component-ified version of the former ATerrainGridActor. Any Actor can
* opt into grid / height functionality by adding this component.
*/
UCLASS(ClassGroup = (Grid), BlueprintType, meta = (BlueprintSpawnableComponent))
class DEMOROUNDBASEDTACTIC_API UHeightMapGridBindingComponent : public USceneComponent
{
	GENERATED_BODY()


public:
	UHeightMapGridBindingComponent();


	/** Height map asset that provides Width / Height and per-cell ground heights. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Terrain")
	UTerrainHeightMapAsset* HeightMapAsset = nullptr;


	/** Physical size of a single cell along each grid axis (in world units). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid")
	float CellSize = 100.f;


	/** World-space centre of the whole grid map. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid")
	FVector GridOrigin = FVector::ZeroVector;


	/** If true, grid orientation is derived from GridRotation; otherwise AxisX / AxisY are used. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Orientation")
	bool bUseRotation = false;


	/** Rotation used to derive the grid basis when bUseRotation is true. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Orientation", meta = (EditCondition = "bUseRotation"))
	FRotator GridRotation = FRotator::ZeroRotator;


	/** Explicit basis vector for the grid X axis when not using rotation. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Orientation", meta = (EditCondition = "!bUseRotation"))
	FVector AxisX = FVector::ForwardVector;


	/** Explicit basis vector for the grid Y axis when not using rotation. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Orientation", meta = (EditCondition = "!bUseRotation"))
	FVector AxisY = FVector::RightVector;


	/** Default eye height above the ground, forwarded into GridConfig. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid|Height")
	float DefaultEyeHeight = 160.f;


	/**
	* Runtime grid configuration built from the asset and the component properties.
	*
	* Only the UPROPERTY fields of FGridConfig are visible to Blueprints; the
	* HeightProvider pointer is injected at runtime for C++ systems.
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid|Runtime")
	FGridConfig GridConfig;


	/** Rebuild the GridConfig from the current properties and HeightMapAsset. */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Grid")
	void RebuildGridConfig();


	/** Blueprint-friendly accessor that returns a copy of the current grid config. */
	UFUNCTION(BlueprintPure, Category = "Grid")
	FGridConfig GetGridConfig() const { return GridConfig; }


protected:
	// Called when the component is registered with the world (both in editor and at runtime).
	virtual void OnRegister() override;
};