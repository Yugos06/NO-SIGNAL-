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
    bBunkerUnlocked = false;
    bLoreRevealed = false;

    InitRelays();
    InitStoryState();

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

    LogAndScreen(TEXT("NO SIGNAL // MARS INCIDENT - Your shuttle crashed. Restore power and reach the bunker."), FColor(60, 180, 255), 8.0f);
    LogAndScreen(TEXT("Controls: TAB Scan | E Repair Node | F Boost Core | R Rest | 1-4 Move Sector"), FColor(190, 190, 190), 8.0f);
    LogAndScreen(BuildHudStatus(), FColor::Yellow, 8.0f);
}

bool ANoSignalGameModeBase::ScanNetwork()
{
    if (bGameOver)
    {
        return false;
    }

    UE_LOG(LogNoSignal, Log, TEXT("Mars outpost scan:"));
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

    LogAndScreen(TEXT("Scan complete. Check Output Log for sector diagnostics."), FColor::White, 4.0f);
    if (bBunkerUnlocked && !bLoreRevealed && CurrentRelayIndex == 3)
    {
        TriggerLoreReveal();
    }
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
        LogAndScreen(TEXT("Invalid sector index."), FColor::Red, 4.0f);
    }
    else if (Energy < 5)
    {
        LogAndScreen(TEXT("Not enough energy to travel."), FColor::Red, 4.0f);
    }
    else if (NewRelayIndex == CurrentRelayIndex)
    {
        LogAndScreen(TEXT("Already in that sector."), FColor(190, 190, 190), 4.0f);
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
        LogAndScreen(TEXT("No active sector selected."), FColor::Red, 4.0f);
    }
    else if (Energy < 12 || SpareParts <= 0)
    {
        LogAndScreen(TEXT("Cannot repair node: missing energy or spare parts."), FColor::Red, 4.0f);
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
            FString::Printf(TEXT("%s node repaired: +%d stability, -%d interference."), *Relay->Name, Gain, Clean),
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
        LogAndScreen(TEXT("Cannot boost bunker core: need 1 battery pack and 10 energy."), FColor::Red, 4.0f);
    }
    else
    {
        const int32 Gain = FMath::RandRange(15, 30);
        BatteryPacks -= 1;
        Energy -= 10;
        HubCharge = Clamp100(HubCharge + Gain);
        LogAndScreen(FString::Printf(TEXT("Bunker core boosted by +%d."), Gain), FColor::Green, 4.0f);
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
        FString::Printf(TEXT("You hide in the wreck: +%d energy, -%d bunker core charge."), Recover, HubLoss),
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
        TEXT("Turn %d/%d | Energy %d | Spare %d | Batteries %d | Core %d | Signal %d | Sector %s | Obj: %s"),
        Turn,
        MaxTurns,
        Energy,
        SpareParts,
        BatteryPacks,
        HubCharge,
        ComputeSignal(),
        *RelayName,
        *CurrentObjective);
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
    North.Name = TEXT("Crash Site Beacon");
    North.Stability = 50;
    North.Interference = 12;
    North.bOnline = true;
    RelayStates.Add(North);

    FRelayState East;
    East.Name = TEXT("Canyon Repeater");
    East.Stability = 40;
    East.Interference = 20;
    East.bOnline = true;
    RelayStates.Add(East);

    FRelayState South;
    South.Name = TEXT("Ancient Dig Array");
    South.Stability = 35;
    South.Interference = 22;
    South.bOnline = false;
    RelayStates.Add(South);

    FRelayState West;
    West.Name = TEXT("Bunker Gate Relay");
    West.Stability = 30;
    West.Interference = 28;
    West.bOnline = false;
    RelayStates.Add(West);
}

void ANoSignalGameModeBase::InitStoryState()
{
    StoryPhase = EMarsStoryPhase::RestorePower;
    bBunkerUnlocked = false;
    bLoreRevealed = false;
    CurrentObjective = TEXT("Restore power to unlock the bunker gate relay (Signal >= 85 and at least 3 sectors online).");
}

void ANoSignalGameModeBase::UpdateStoryState()
{
    if (!bBunkerUnlocked)
    {
        const int32 Signal = ComputeSignal();
        const int32 OnlineCount = GetOnlineRelayCount();
        if (Signal >= 85 && OnlineCount >= 3)
        {
            bBunkerUnlocked = true;
            StoryPhase = EMarsStoryPhase::ReachBunker;
            CurrentObjective = TEXT("Bunker gate unlocked. Travel to Bunker Gate Relay and scan to access the archive.");
            LogAndScreen(TEXT("Power restored. Bunker gate relay is now unlocked."), FColor::Green, 6.0f);
            return;
        }
    }

    if (!bLoreRevealed)
    {
        if (bBunkerUnlocked && CurrentRelayIndex == 3)
        {
            CurrentObjective = TEXT("At bunker gate relay. Run a scan to decode and unlock archive access.");
        }
        else if (bBunkerUnlocked)
        {
            CurrentObjective = TEXT("Travel to Bunker Gate Relay and run a scan to access the archive.");
        }
    }
}

void ANoSignalGameModeBase::TriggerLoreReveal()
{
    if (bLoreRevealed)
    {
        return;
    }

    bLoreRevealed = true;
    StoryPhase = EMarsStoryPhase::ArchiveRevealed;
    CurrentObjective = TEXT("Archive decrypted. Transmit proof of the ancient Martian civilization.");

    LogAndScreen(
        TEXT("Archive entry: The old civilization collapsed after contact with a signal-borne entity."),
        FColor(180, 220, 255),
        8.0f);
}

void ANoSignalGameModeBase::AdvanceTurn()
{
    if (bGameOver)
    {
        return;
    }

    UpdateStoryState();
    SyncActorsFromState();

    if (CheckEndConditions())
    {
        return;
    }

    RandomEvent();
    Turn += 1;

    UpdateStoryState();
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
            FString::Printf(TEXT("[Event] Unknown entity pulse at %s (+%d interference)."), *Relay.Name, InterferenceGain),
            FColor(255, 165, 40),
            4.0f);
    }
    else if (EventRoll < 80)
    {
        const int32 Loss = FMath::RandRange(4, 12);
        HubCharge = Clamp100(HubCharge - Loss);
        LogAndScreen(FString::Printf(TEXT("[Event] Bunker core leak (-%d charge)."), Loss), FColor(255, 165, 40), 4.0f);
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

        LogAndScreen(FString::Printf(TEXT("[Event] Old maintenance drones patched %s."), *WeakestRelay.Name), FColor(80, 220, 120), 4.0f);
    }
    else
    {
        SpareParts += 1;
        LogAndScreen(TEXT("[Event] You found a sealed crate in the ruins."), FColor(80, 220, 120), 4.0f);
    }
}

bool ANoSignalGameModeBase::CheckEndConditions()
{
    if (bLoreRevealed)
    {
        bGameOver = true;
        LogAndScreen(TEXT("Victory: archive recovered. You confirm an ancient Martian civilization."), FColor::Green, 10.0f);
        return true;
    }

    const int32 Signal = ComputeSignal();

    if (Turn > MaxTurns)
    {
        bGameOver = true;
        LogAndScreen(TEXT("Defeat: oxygen reserves depleted before reaching the bunker."), FColor::Red, 10.0f);
        return true;
    }

    if (Signal <= 10 && Turn > 3)
    {
        bGameOver = true;
        LogAndScreen(TEXT("Defeat: signal lost. Something in the storm found you first."), FColor::Red, 10.0f);
        return true;
    }

    if (Energy <= 0 && SpareParts <= 0 && BatteryPacks <= 0)
    {
        bGameOver = true;
        LogAndScreen(TEXT("Defeat: no resources left. The dark swallowed the outpost."), FColor::Red, 10.0f);
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
