#include "HeightMapGridBindingComponent.h"

#include "Engine/World.h"
#include "GameFramework/Actor.h"

// Anonymous namespace for a simple array-backed height provider implementation.
namespace
{
    class FArrayGridHeightProvider final : public IGridHeightProvider
    {
    public:
        FArrayGridHeightProvider(int32 InWidth,
            int32 InHeight,
            const TArray<float>& InCellHeights)
            : Width(InWidth)
            , Height(InHeight)
            , CellHeights(InCellHeights) // copy into an internal buffer
        {
#if DO_CHECK
            check(CellHeights.Num() == Width * Height);
#endif
        }

        virtual float GetHeightAt(int32 GridX, int32 GridY) const override
        {
            const int32 Index = GridY * Width + GridX;
#if DO_CHECK
            check(Index >= 0 && Index < CellHeights.Num());
#endif
            return CellHeights[Index];
        }

    private:
        int32 Width = 0;
        int32 Height = 0;
        TArray<float> CellHeights;
    };
}

UHeightMapGridBindingComponent::UHeightMapGridBindingComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UHeightMapGridBindingComponent::OnRegister()
{
    Super::OnRegister();

    // Build an initial config whenever the component is registered.
    RebuildGridConfig();
}

void UHeightMapGridBindingComponent::RebuildGridConfig()
{
    // Reset to a clean config first.
    GridConfig = FGridConfig{};

    if (!HeightMapAsset)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("HeightMapGridBindingComponent '%s' on '%s' has no HeightMapAsset set."),
            *GetName(),
            GetOwner() ? *GetOwner()->GetName() : TEXT("<no owner>"));
        return;
    }

    if (HeightMapAsset->Width <= 0 || HeightMapAsset->Height <= 0 ||
        HeightMapAsset->CellHeights.Num() != HeightMapAsset->Width * HeightMapAsset->Height)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("HeightMapGridBindingComponent '%s' has invalid HeightMapAsset '%s' (Width=%d, Height=%d, NumHeights=%d)."),
            *GetName(),
            *HeightMapAsset->GetName(),
            HeightMapAsset->Width,
            HeightMapAsset->Height,
            HeightMapAsset->CellHeights.Num());
        return;
    }

    // Shape
    GridConfig.Width = HeightMapAsset->Width;
    GridConfig.Height = HeightMapAsset->Height;
    GridConfig.CellSize = CellSize;

    // Position / orientation
    GridConfig.bUseRotation = bUseRotation;
    GridConfig.GridRotation = GridRotation;


    FVector BasisX;
    FVector BasisY;

    if (GridConfig.bUseRotation)
    {
        const FRotationMatrix R(GridConfig.GridRotation);
        BasisX = R.GetUnitAxis(EAxis::X);
        BasisY = R.GetUnitAxis(EAxis::Y);

        GridConfig.AxisX = BasisX;
        GridConfig.AxisY = BasisY;
    }
    else
    {
        BasisX = AxisX;
        BasisY = AxisY;

        BasisX.Z = 0.0f;
        BasisY.Z = 0.0f;

        if (!BasisX.Normalize())
        {
            BasisX = FVector::ForwardVector;
        }
        if (!BasisY.Normalize())
        {
            BasisY = FVector::RightVector;
        }

        GridConfig.AxisX = BasisX;
        GridConfig.AxisY = BasisY;
    }

    // --------------------------
    // 3. 把“地图中心”转换成几何库所需的“左下角角点”
    // --------------------------
    const FVector MapCenterWS = GridOrigin; // 组件上的 GridOrigin 始终表示“整张地图的中心”

    const float HalfSizeX = 0.5f * static_cast<float>(GridConfig.Width)  * GridConfig.CellSize;
    const float HalfSizeY = 0.5f * static_cast<float>(GridConfig.Height) * GridConfig.CellSize;

    // 这里得到的是“左下角角点”的世界坐标
    GridConfig.GridOrigin = MapCenterWS - BasisX * HalfSizeX - BasisY * HalfSizeY;

    // --------------------------
    // 4. 高度配置
    // --------------------------
    GridConfig.DefaultEyeHeight = DefaultEyeHeight;

    // Inject a runtime height provider backed by the asset data.
    GridConfig.HeightProvider = MakeShared<FArrayGridHeightProvider>(
        HeightMapAsset->Width,
        HeightMapAsset->Height,
        HeightMapAsset->CellHeights);
}
