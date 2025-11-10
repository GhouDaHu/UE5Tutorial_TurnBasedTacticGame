#include "HalfLifeSmootherComponent.h"
#include "Math/UnrealMathUtility.h"


static FORCEINLINE float ExpSmoothHL_Component(float Current, float Desire, float HalfLife, float dt)
{
	if (HalfLife <= SMALL_NUMBER) return Desire;
	const float Lambda = 0.6931471805599453f / HalfLife; // ln(2)/T
	const float OneMinusS = 1.f - FMath::Exp(-Lambda * FMath::Max(0.f, dt));
	return Current + (Desire - Current) * OneMinusS;
}


UHalfLifeSmootherComponent::UHalfLifeSmootherComponent()
{
	PrimaryComponentTick.bCanEverTick = false; // Call Update(dt) from BP or C++ Tick
}


void UHalfLifeSmootherComponent::SetDesire(float NewDesire)
{
	Desire = bClamp ? FMath::Clamp(NewDesire, MinValue, MaxValue) : NewDesire;
}


void UHalfLifeSmootherComponent::SetCurrent(float NewCurrent)
{
	Current = bClamp ? FMath::Clamp(NewCurrent, MinValue, MaxValue) : NewCurrent;
}


float UHalfLifeSmootherComponent::Update(float DeltaTime)
{
	if (bClamp)
	{
		Desire = FMath::Clamp(Desire, MinValue, MaxValue);
		Current = FMath::Clamp(Current, MinValue, MaxValue);
	}
	Current = ExpSmoothHL_Component(Current, Desire, HalfLife, DeltaTime);
	if (bClamp) Current = FMath::Clamp(Current, MinValue, MaxValue);
	return Current;
}