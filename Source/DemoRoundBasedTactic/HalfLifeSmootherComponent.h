#pragma once
#include "Components/ActorComponent.h"
#include "HalfLifeSmootherComponent.generated.h"


// Tiny stateful smoother: holds Current/Desire; call Update(dt) each Tick
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DEMOROUNDBASEDTACTIC_API UHalfLifeSmootherComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Smoothing") float HalfLife = 0.12f; // 0.08~0.20 good
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Smoothing") float Current = 600.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Smoothing") float Desire = 600.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Smoothing") float MinValue = 150.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Smoothing") float MaxValue = 1000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Smoothing") bool bClamp = true;


	UHalfLifeSmootherComponent();


	UFUNCTION(BlueprintCallable, Category = "Smoothing") void SetDesire(float NewDesire);
	UFUNCTION(BlueprintCallable, Category = "Smoothing") void SetCurrent(float NewCurrent);
	UFUNCTION(BlueprintCallable, Category = "Smoothing") float Update(float DeltaTime); // returns Current
};