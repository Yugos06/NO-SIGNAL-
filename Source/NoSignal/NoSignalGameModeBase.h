#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NoSignalGameModeBase.generated.h"

class ANoSignalPlayerPawn;
class ASignalRelayActor;

USTRUCT(BlueprintType)
struct FRelayState
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relay")
    FString Name = TEXT("Relay");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relay")
    int32 Stability = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relay")
    int32 Interference = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relay")
    bool bOnline = false;
};

UCLASS()
class NOSIGNAL_API ANoSignalGameModeBase : public AGameModeBase
{
    GENERATED_BODY()

public:
    ANoSignalGameModeBase();

    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category = "NoSignal|Actions")
    bool ScanNetwork();

    UFUNCTION(BlueprintCallable, Category = "NoSignal|Actions")
    bool TravelToRelay(int32 NewRelayIndex);

    UFUNCTION(BlueprintCallable, Category = "NoSignal|Actions")
    bool RepairCurrentRelay();

    UFUNCTION(BlueprintCallable, Category = "NoSignal|Actions")
    bool BoostHub();

    UFUNCTION(BlueprintCallable, Category = "NoSignal|Actions")
    bool Rest();

    UFUNCTION(BlueprintPure, Category = "NoSignal|State")
    int32 ComputeSignal() const;

    UFUNCTION(BlueprintPure, Category = "NoSignal|State")
    int32 GetOnlineRelayCount() const;

    UFUNCTION(BlueprintPure, Category = "NoSignal|State")
    FString BuildHudStatus() const;

    UFUNCTION(BlueprintPure, Category = "NoSignal|State")
    bool IsGameOver() const { return bGameOver; }

    UFUNCTION(BlueprintPure, Category = "NoSignal|State")
    int32 GetCurrentRelayIndex() const { return CurrentRelayIndex; }

    UPROPERTY(BlueprintReadOnly, Category = "NoSignal|State")
    int32 Turn = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NoSignal|Rules")
    int32 MaxTurns = 20;

    UPROPERTY(BlueprintReadOnly, Category = "NoSignal|State")
    int32 Energy = 75;

    UPROPERTY(BlueprintReadOnly, Category = "NoSignal|State")
    int32 SpareParts = 4;

    UPROPERTY(BlueprintReadOnly, Category = "NoSignal|State")
    int32 BatteryPacks = 3;

    UPROPERTY(BlueprintReadOnly, Category = "NoSignal|State")
    int32 HubCharge = 30;

    UPROPERTY(BlueprintReadOnly, Category = "NoSignal|State")
    int32 CurrentRelayIndex = 0;

    UPROPERTY(BlueprintReadOnly, Category = "NoSignal|State")
    bool bGameOver = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NoSignal|Debug")
    bool bPrintOnScreen = true;

    UPROPERTY(BlueprintReadOnly, Category = "NoSignal|State")
    TArray<FRelayState> RelayStates;

private:
    UPROPERTY()
    TArray<TObjectPtr<ASignalRelayActor>> RelayActors;

    int32 Clamp100(int32 Value) const;
    FRelayState* GetRelayStateByIndex(int32 Index);
    const FRelayState* GetRelayStateByIndex(int32 Index) const;

    void InitRelays();
    void AdvanceTurn();
    void RandomEvent();
    bool CheckEndConditions();
    void SyncActorsFromState();
    void LogAndScreen(const FString& Message, FColor Color = FColor::White, float Duration = 4.0f) const;
};
