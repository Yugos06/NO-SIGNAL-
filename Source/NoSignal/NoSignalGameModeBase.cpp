#include "NoSignalGameModeBase.h"

#include "Engine/Engine.h"
#include "EngineUtils.h"
#include "NoSignal.h"
#include "NoSignalPlayerPawn.h"
#include "SignalRelayActor.h"

ANoSignalGameModeBase::ANoSignalGameModeBase()
{
    DefaultPawnClass = ANoSignalPlayerPawn::StaticClass();
}

void ANoSignalGameModeBase::BeginPlay()
{
    Super::BeginPlay();

    Turn = 1;
    MaxTurns = FMath::Max(MaxTurns, 1);
    Energy = 75;
    SpareParts = 4;
    BatteryPacks = 3;
    HubCharge = 30;
    CurrentRelayIndex = 0;
    bGameOver = false;

    InitRelays();

    RelayActors.Empty();
    for (TActorIterator<ASignalRelayActor> It(GetWorld()); It; ++It)
    {
        RelayActors.Add(*It);
    }

    RelayActors.Sort([](const TObjectPtr<ASignalRelayActor>& A, const TObjectPtr<ASignalRelayActor>& B)
    {
        if (!A)
        {
            return false;
        }
        if (!B)
        {
            return true;
        }
        return A->RelayIndex < B->RelayIndex;
    });

    SyncActorsFromState();

    LogAndScreen(TEXT("NO SIGNAL V0.1 UE - Survive and restore signal before turn limit."), FColor(60, 180, 255), 8.0f);
    LogAndScreen(TEXT("Controls: TAB Scan | E Repair | F Boost Hub | R Rest | 1-4 Travel"), FColor(190, 190, 190), 8.0f);
    LogAndScreen(BuildHudStatus(), FColor::Yellow, 8.0f);
}

bool ANoSignalGameModeBase::ScanNetwork()
{
    if (bGameOver)
    {
        return false;
    }

    UE_LOG(LogNoSignal, Log, TEXT("Network scan:"));
    for (int32 i = 0; i < RelayStates.Num(); ++i)
    {
        const FRelayState& Relay = RelayStates[i];
        UE_LOG(
            LogNoSignal,
            Log,
            TEXT("Relay %d - %s | State: %s | Stability: %d | Interference: %d"),
            i + 1,
            *Relay.Name,
            Relay.bOnline ? TEXT("ONLINE") : TEXT("OFFLINE"),
            Relay.Stability,
            Relay.Interference);
    }

    LogAndScreen(TEXT("Scan complete. See Output Log for detailed relay stats."), FColor::White, 4.0f);
    AdvanceTurn();
    return true;
}

bool ANoSignalGameModeBase::TravelToRelay(const int32 NewRelayIndex)
{
    if (bGameOver)
    {
        return false;
    }

    bool bSuccess = false;
    if (!RelayStates.IsValidIndex(NewRelayIndex))
    {
        LogAndScreen(TEXT("Invalid relay index."), FColor::Red, 4.0f);
    }
    else if (Energy < 5)
    {
        LogAndScreen(TEXT("Not enough energy to travel."), FColor::Red, 4.0f);
    }
    else if (NewRelayIndex == CurrentRelayIndex)
    {
        LogAndScreen(TEXT("Already at that relay."), FColor(190, 190, 190), 4.0f);
    }
    else
    {
        Energy -= 5;
        CurrentRelayIndex = NewRelayIndex;
        LogAndScreen(FString::Printf(TEXT("Moved to %s."), *RelayStates[CurrentRelayIndex].Name), FColor::Green, 4.0f);
        bSuccess = true;
    }

    AdvanceTurn();
    return bSuccess;
}

bool ANoSignalGameModeBase::RepairCurrentRelay()
{
    if (bGameOver)
    {
        return false;
    }

    bool bSuccess = false;
    FRelayState* Relay = GetRelayStateByIndex(CurrentRelayIndex);
    if (!Relay)
    {
        LogAndScreen(TEXT("No active relay selected."), FColor::Red, 4.0f);
    }
    else if (Energy < 12 || SpareParts <= 0)
    {
        LogAndScreen(TEXT("Cannot repair: missing energy or spare parts."), FColor::Red, 4.0f);
    }
    else
    {
        const int32 Gain = FMath::RandRange(12, 25);
        const int32 Clean = FMath::RandRange(4, 15);

        Energy -= 12;
        SpareParts -= 1;
        Relay->Stability = Clamp100(Relay->Stability + Gain);
        Relay->Interference = Clamp100(Relay->Interference - Clean);
        Relay->bOnline = Relay->Stability >= 35;

        LogAndScreen(
            FString::Printf(TEXT("%s repaired: +%d stability, -%d interference."), *Relay->Name, Gain, Clean),
            FColor::Green,
            4.0f);

        bSuccess = true;
    }

    AdvanceTurn();
    return bSuccess;
}

bool ANoSignalGameModeBase::BoostHub()
{
    if (bGameOver)
    {
        return false;
    }

    bool bSuccess = false;
    if (BatteryPacks <= 0 || Energy < 10)
    {
        LogAndScreen(TEXT("Cannot boost: need 1 battery pack and 10 energy."), FColor::Red, 4.0f);
    }
    else
    {
        const int32 Gain = FMath::RandRange(15, 30);
        BatteryPacks -= 1;
        Energy -= 10;
        HubCharge = Clamp100(HubCharge + Gain);
        LogAndScreen(FString::Printf(TEXT("Hub boosted by +%d."), Gain), FColor::Green, 4.0f);
        bSuccess = true;
    }

    AdvanceTurn();
    return bSuccess;
}

bool ANoSignalGameModeBase::Rest()
{
    if (bGameOver)
    {
        return false;
    }

    const int32 Recover = FMath::RandRange(12, 22);
    const int32 HubLoss = FMath::RandRange(3, 8);

    Energy = Clamp100(Energy + Recover);
    HubCharge = Clamp100(HubCharge - HubLoss);
    LogAndScreen(
        FString::Printf(TEXT("Rested: +%d energy, -%d hub charge."), Recover, HubLoss),
        FColor(190, 190, 190),
        4.0f);

    AdvanceTurn();
    return true;
}

int32 ANoSignalGameModeBase::ComputeSignal() const
{
    if (RelayStates.IsEmpty())
    {
        return 0;
    }

    int32 StabilitySum = 0;
    int32 OnlineCount = 0;
    for (const FRelayState& Relay : RelayStates)
    {
        const int32 EffectiveStability = Clamp100(Relay.Stability - (Relay.Interference / 2));
        StabilitySum += EffectiveStability;
        if (Relay.bOnline)
        {
            OnlineCount += 1;
        }
    }

    const int32 AverageRelay = StabilitySum / RelayStates.Num();
    int32 Signal = (AverageRelay * 65 + HubCharge * 35) / 100;
    Signal -= (RelayStates.Num() - OnlineCount) * 7;
    return Clamp100(Signal);
}

int32 ANoSignalGameModeBase::GetOnlineRelayCount() const
{
    int32 Count = 0;
    for (const FRelayState& Relay : RelayStates)
    {
        if (Relay.bOnline)
        {
            Count += 1;
        }
    }
    return Count;
}

FString ANoSignalGameModeBase::BuildHudStatus() const
{
    const FString RelayName = RelayStates.IsValidIndex(CurrentRelayIndex)
        ? RelayStates[CurrentRelayIndex].Name
        : TEXT("Unknown");

    return FString::Printf(
        TEXT("Turn %d/%d | Energy %d | Spare %d | Batteries %d | Hub %d | Signal %d | Relay %s"),
        Turn,
        MaxTurns,
        Energy,
        SpareParts,
        BatteryPacks,
        HubCharge,
        ComputeSignal(),
        *RelayName);
}

int32 ANoSignalGameModeBase::Clamp100(const int32 Value) const
{
    return FMath::Clamp(Value, 0, 100);
}

FRelayState* ANoSignalGameModeBase::GetRelayStateByIndex(const int32 Index)
{
    return RelayStates.IsValidIndex(Index) ? &RelayStates[Index] : nullptr;
}

const FRelayState* ANoSignalGameModeBase::GetRelayStateByIndex(const int32 Index) const
{
    return RelayStates.IsValidIndex(Index) ? &RelayStates[Index] : nullptr;
}

void ANoSignalGameModeBase::InitRelays()
{
    RelayStates.Empty();

    FRelayState North;
    North.Name = TEXT("North Tower");
    North.Stability = 50;
    North.Interference = 12;
    North.bOnline = true;
    RelayStates.Add(North);

    FRelayState East;
    East.Name = TEXT("East Tower");
    East.Stability = 40;
    East.Interference = 20;
    East.bOnline = true;
    RelayStates.Add(East);

    FRelayState South;
    South.Name = TEXT("South Tower");
    South.Stability = 35;
    South.Interference = 22;
    South.bOnline = false;
    RelayStates.Add(South);

    FRelayState West;
    West.Name = TEXT("West Tower");
    West.Stability = 30;
    West.Interference = 28;
    West.bOnline = false;
    RelayStates.Add(West);
}

void ANoSignalGameModeBase::AdvanceTurn()
{
    if (bGameOver)
    {
        return;
    }

    SyncActorsFromState();

    if (CheckEndConditions())
    {
        return;
    }

    RandomEvent();
    Turn += 1;

    SyncActorsFromState();
    CheckEndConditions();

    if (!bGameOver)
    {
        LogAndScreen(BuildHudStatus(), FColor::Yellow, 2.5f);
    }
}

void ANoSignalGameModeBase::RandomEvent()
{
    const int32 EventRoll = FMath::RandRange(0, 99);
    if (RelayStates.IsEmpty())
    {
        return;
    }

    if (EventRoll < 50)
    {
        const int32 RelayIdx = FMath::RandRange(0, RelayStates.Num() - 1);
        FRelayState& Relay = RelayStates[RelayIdx];

        const int32 InterferenceGain = FMath::RandRange(5, 14);
        const int32 StabilityLoss = FMath::RandRange(0, 6);

        Relay.Interference = Clamp100(Relay.Interference + InterferenceGain);
        Relay.Stability = Clamp100(Relay.Stability - StabilityLoss);
        if (Relay.Stability < 25)
        {
            Relay.bOnline = false;
        }

        LogAndScreen(
            FString::Printf(TEXT("[Event] Distortion spike at %s (+%d interference)."), *Relay.Name, InterferenceGain),
            FColor(255, 165, 40),
            4.0f);
    }
    else if (EventRoll < 80)
    {
        const int32 Loss = FMath::RandRange(4, 12);
        HubCharge = Clamp100(HubCharge - Loss);
        LogAndScreen(FString::Printf(TEXT("[Event] Central hub leak (-%d charge)."), Loss), FColor(255, 165, 40), 4.0f);
    }
    else if (EventRoll < 92)
    {
        int32 WeakestIndex = 0;
        int32 WeakestStability = RelayStates[0].Stability;
        for (int32 i = 1; i < RelayStates.Num(); ++i)
        {
            if (RelayStates[i].Stability < WeakestStability)
            {
                WeakestStability = RelayStates[i].Stability;
                WeakestIndex = i;
            }
        }

        FRelayState& WeakestRelay = RelayStates[WeakestIndex];
        WeakestRelay.Stability = Clamp100(WeakestRelay.Stability + 10);
        if (WeakestRelay.Stability >= 35)
        {
            WeakestRelay.bOnline = true;
        }

        LogAndScreen(FString::Printf(TEXT("[Event] Civilians patched %s."), *WeakestRelay.Name), FColor(80, 220, 120), 4.0f);
    }
    else
    {
        SpareParts += 1;
        LogAndScreen(TEXT("[Event] You found an abandoned spare part cache."), FColor(80, 220, 120), 4.0f);
    }
}

bool ANoSignalGameModeBase::CheckEndConditions()
{
    const int32 Signal = ComputeSignal();
    const int32 OnlineCount = GetOnlineRelayCount();

    if (Signal >= 85 && OnlineCount >= 3)
    {
        bGameOver = true;
        LogAndScreen(TEXT("Victory: the city is back online."), FColor::Green, 10.0f);
        return true;
    }

    if (Turn > MaxTurns)
    {
        bGameOver = true;
        LogAndScreen(TEXT("Defeat: time is up. The network collapsed."), FColor::Red, 10.0f);
        return true;
    }

    if (Signal <= 10 && Turn > 3)
    {
        bGameOver = true;
        LogAndScreen(TEXT("Defeat: signal fully lost. No recovery possible."), FColor::Red, 10.0f);
        return true;
    }

    if (Energy <= 0 && SpareParts <= 0 && BatteryPacks <= 0)
    {
        bGameOver = true;
        LogAndScreen(TEXT("Defeat: no resources left to continue."), FColor::Red, 10.0f);
        return true;
    }

    return false;
}

void ANoSignalGameModeBase::SyncActorsFromState()
{
    for (ASignalRelayActor* RelayActor : RelayActors)
    {
        if (!IsValid(RelayActor))
        {
            continue;
        }

        if (const FRelayState* State = GetRelayStateByIndex(RelayActor->RelayIndex))
        {
            RelayActor->RelayName = State->Name;
            RelayActor->SetRelayState(State->Stability, State->Interference, State->bOnline);
        }
    }
}

void ANoSignalGameModeBase::LogAndScreen(const FString& Message, const FColor Color, const float Duration) const
{
    UE_LOG(LogNoSignal, Log, TEXT("%s"), *Message);
    if (bPrintOnScreen && GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, Duration, Color, Message);
    }
}
