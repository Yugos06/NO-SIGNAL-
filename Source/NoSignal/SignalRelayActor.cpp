#include "SignalRelayActor.h"

#include "Components/PointLightComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"

ASignalRelayActor::ASignalRelayActor()
{
    PrimaryActorTick.bCanEverTick = false;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);

    TowerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TowerMesh"));
    TowerMesh->SetupAttachment(Root);
    TowerMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    StatusLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("StatusLight"));
    StatusLight->SetupAttachment(Root);
    StatusLight->SetIntensity(3500.0f);
    StatusLight->SetAttenuationRadius(500.0f);

    UpdateVisuals();
}

void ASignalRelayActor::SetRelayState(const int32 NewStability, const int32 NewInterference, const bool bNewOnline)
{
    Stability = FMath::Clamp(NewStability, 0, 100);
    Interference = FMath::Clamp(NewInterference, 0, 100);
    bOnline = bNewOnline;
    UpdateVisuals();
}

void ASignalRelayActor::UpdateVisuals() const
{
    const FColor Color = bOnline ? FColor(80, 255, 140) : FColor(255, 90, 90);
    StatusLight->SetLightColor(Color);

    const float BaseIntensity = bOnline ? 5000.0f : 2000.0f;
    const float StabilityFactor = static_cast<float>(Stability) / 100.0f;
    StatusLight->SetIntensity(BaseIntensity + 2500.0f * StabilityFactor);
}
