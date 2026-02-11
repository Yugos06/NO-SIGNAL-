#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SignalRelayActor.generated.h"

class UPointLightComponent;
class USceneComponent;
class UStaticMeshComponent;

UCLASS()
class NOSIGNAL_API ASignalRelayActor : public AActor
{
    GENERATED_BODY()

public:
    ASignalRelayActor();

    UFUNCTION(BlueprintCallable, Category = "Relay")
    void SetRelayState(int32 NewStability, int32 NewInterference, bool bNewOnline);

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Relay")
    int32 RelayIndex = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Relay")
    FString RelayName = TEXT("Relay");

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Relay|State")
    int32 Stability = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Relay|State")
    int32 Interference = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Relay|State")
    bool bOnline = false;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<USceneComponent> Root;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> TowerMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UPointLightComponent> StatusLight;

private:
    void UpdateVisuals() const;
};
