#include "TerrainHeightMapAsset.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "Engine/Texture.h"              // FTextureSource, TSF_G16
#include "IAssetTools.h"
#include "Misc/PackageName.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"

UTerrainHeightMapAsset* UTerrainHeightMapLibrary::CreateHeightMapAssetFromTexture(
    TSoftObjectPtr<UTexture2D> HeightTexture,
    float WorldZScale)
{
    // Resolve the texture from the soft reference.
    UTexture2D* Texture = HeightTexture.LoadSynchronous();
    if (!Texture)
    {
        UE_LOG(LogTemp, Warning, TEXT("CreateHeightMapAssetFromTexture: HeightTexture is null."));
        return nullptr;
    }

    FTextureSource& Source = Texture->Source;
    if (!Source.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("CreateHeightMapAssetFromTexture: Texture source is invalid."));
        return nullptr;
    }

    if (Source.GetFormat() != TSF_G16)
    {
        UE_LOG(LogTemp, Warning, TEXT("CreateHeightMapAssetFromTexture: Texture source is not TSF_G16 (16-bit grayscale)."));
        return nullptr;
    }

    const int32 Width  = Source.GetSizeX();
    const int32 Height = Source.GetSizeY();
    if (Width <= 0 || Height <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("CreateHeightMapAssetFromTexture: Invalid texture size %dx%d."), Width, Height);
        return nullptr;
    }

    // Access raw 16-bit grayscale data from mip 0.
    const uint8* RawData = Source.LockMipReadOnly(0);
    if (!RawData)
    {
        UE_LOG(LogTemp, Warning, TEXT("CreateHeightMapAssetFromTexture: Failed to lock texture mip 0."));
        return nullptr;
    }

    // Copy and convert to world-space float heights.
    TArray<float> CellHeights;
    CellHeights.SetNumUninitialized(Width * Height);

    const uint16* Pixels = reinterpret_cast<const uint16*>(RawData);

    for (int32 Y = 0; Y < Height; ++Y)
    {
        for (int32 X = 0; X < Width; ++X)
        {
            const int32 Index   = Y * Width + X;
            const uint16 Sample = Pixels[Index];

            const float Normalized = static_cast<float>(Sample) / 65535.0f; // [0,1]
            const float WorldZ     = Normalized * WorldZScale;

            CellHeights[Index] = WorldZ;
        }
    }

    Source.UnlockMip(0);

    // Derive folder and base asset name from the texture's package and asset name.
    const FString SourcePackageName = Texture->GetOutermost()->GetName(); // e.g. "/Game/Terrain/HeightMaps/T_Height_S42_256x256"
    const FString FolderPath        = FPackageName::GetLongPackagePath(SourcePackageName); // e.g. "/Game/Terrain/HeightMaps"
    const FString TextureAssetName  = Texture->GetName(); // e.g. "T_Height_S42_256x256"
    const FString BaseName          = FString::Printf(TEXT("DA_%s"), *TextureAssetName); // e.g. "DA_T_Height_S42_256x256"

    if (FolderPath.IsEmpty() || BaseName.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("CreateHeightMapAssetFromTexture: Failed to derive folder or base name from texture '%s'."), *SourcePackageName);
        return nullptr;
    }

    // Use AssetTools to create a unique package + asset name.
    FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");

    FString PackageName;
    FString AssetName;
    AssetToolsModule.Get().CreateUniqueAssetName(
        FolderPath / BaseName,
        TEXT(""),
        PackageName,
        AssetName
    );

    UPackage* Package = CreatePackage(*PackageName);
    if (!Package)
    {
        UE_LOG(LogTemp, Warning, TEXT("CreateHeightMapAssetFromTexture: Failed to create package '%s'."), *PackageName);
        return nullptr;
    }

    // Create the asset object inside the package.
    UTerrainHeightMapAsset* NewAsset = NewObject<UTerrainHeightMapAsset>(
        Package,
        *AssetName,
        RF_Public | RF_Standalone
    );

    if (!NewAsset)
    {
        UE_LOG(LogTemp, Warning, TEXT("CreateHeightMapAssetFromTexture: Failed to create UTerrainHeightMapAsset in package '%s'."), *PackageName);
        return nullptr;
    }

    // Fill asset data.
    NewAsset->Width       = Width;
    NewAsset->Height      = Height;
    NewAsset->CellHeights = MoveTemp(CellHeights);

    // Inform asset registry and mark package dirty.
    FAssetRegistryModule::AssetCreated(NewAsset);
    Package->MarkPackageDirty();

    // Save the package immediately so that the .uasset appears on disk.
    const FString FilePath = FPackageName::LongPackageNameToFilename(
        PackageName,
        FPackageName::GetAssetPackageExtension()
    );

    FSavePackageArgs SaveArgs;
    SaveArgs.TopLevelFlags         = RF_Public | RF_Standalone;
    SaveArgs.Error                 = GError;
    SaveArgs.bWarnOfLongFilename   = false;

    const bool bSuccess = UPackage::SavePackage(Package, NewAsset, *FilePath, SaveArgs);
    if (!bSuccess)
    {
        UE_LOG(LogTemp, Warning, TEXT("CreateHeightMapAssetFromTexture: Failed to save package '%s' to '%s'."), *PackageName, *FilePath);
    }

    return NewAsset;
}
