#include "NoSignalPlayerPawn.h"

#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"
#include "NoSignalGameModeBase.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"

ANoSignalPlayerPawn::ANoSignalPlayerPawn()
{
    PrimaryActorTick.bCanEverTick = false;
    AutoPossessPlayer = EAutoReceiveInput::Player0;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);

    BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
    BodyMesh->SetupAttachment(Root);
    BodyMesh->SetCollisionProfileName(TEXT("Pawn"));

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(Root);
    SpringArm->TargetArmLength = 420.0f;
    SpringArm->bUsePawnControlRotation = true;
    SpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 140.0f));

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

    Movement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Movement"));
    Movement->MaxSpeed = 1150.0f;
    Movement->Acceleration = 3200.0f;
    Movement->Deceleration = 3600.0f;
}

void ANoSignalPlayerPawn::BeginPlay()
{
    Super::BeginPlay();
    PrintControlHints();
}

void ANoSignalPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    check(PlayerInputComponent);

    PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ANoSignalPlayerPawn::MoveForward);
    PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ANoSignalPlayerPawn::MoveRight);
    PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ANoSignalPlayerPawn::TurnYaw);
    PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ANoSignalPlayerPawn::LookUpPitch);

    PlayerInputComponent->BindAction(TEXT("Scan"), IE_Pressed, this, &ANoSignalPlayerPawn::HandleScan);
    PlayerInputComponent->BindAction(TEXT("Repair"), IE_Pressed, this, &ANoSignalPlayerPawn::HandleRepair);
    PlayerInputComponent->BindAction(TEXT("BoostHub"), IE_Pressed, this, &ANoSignalPlayerPawn::HandleBoostHub);
    PlayerInputComponent->BindAction(TEXT("Rest"), IE_Pressed, this, &ANoSignalPlayerPawn::HandleRest);

    PlayerInputComponent->BindAction(TEXT("Relay1"), IE_Pressed, this, &ANoSignalPlayerPawn::SelectRelay1);
    PlayerInputComponent->BindAction(TEXT("Relay2"), IE_Pressed, this, &ANoSignalPlayerPawn::SelectRelay2);
    PlayerInputComponent->BindAction(TEXT("Relay3"), IE_Pressed, this, &ANoSignalPlayerPawn::SelectRelay3);
    PlayerInputComponent->BindAction(TEXT("Relay4"), IE_Pressed, this, &ANoSignalPlayerPawn::SelectRelay4);
}

void ANoSignalPlayerPawn::MoveForward(const float Value)
{
    if (FMath::IsNearlyZero(Value))
    {
        return;
    }
    AddMovementInput(GetActorForwardVector(), Value);
}

void ANoSignalPlayerPawn::MoveRight(const float Value)
{
    if (FMath::IsNearlyZero(Value))
    {
        return;
    }
    AddMovementInput(GetActorRightVector(), Value);
}

void ANoSignalPlayerPawn::TurnYaw(const float Value)
{
    AddControllerYawInput(Value);
}

void ANoSignalPlayerPawn::LookUpPitch(const float Value)
{
    AddControllerPitchInput(Value);
}

void ANoSignalPlayerPawn::HandleScan()
{
    if (ANoSignalGameModeBase* GM = GetWorld() ? GetWorld()->GetAuthGameMode<ANoSignalGameModeBase>() : nullptr)
    {
        GM->ScanNetwork();
    }
}

void ANoSignalPlayerPawn::HandleRepair()
{
    if (ANoSignalGameModeBase* GM = GetWorld() ? GetWorld()->GetAuthGameMode<ANoSignalGameModeBase>() : nullptr)
    {
        GM->RepairCurrentRelay();
    }
}

void ANoSignalPlayerPawn::HandleBoostHub()
{
    if (ANoSignalGameModeBase* GM = GetWorld() ? GetWorld()->GetAuthGameMode<ANoSignalGameModeBase>() : nullptr)
    {
        GM->BoostHub();
    }
}

void ANoSignalPlayerPawn::HandleRest()
{
    if (ANoSignalGameModeBase* GM = GetWorld() ? GetWorld()->GetAuthGameMode<ANoSignalGameModeBase>() : nullptr)
    {
        GM->Rest();
    }
}

void ANoSignalPlayerPawn::SelectRelay1()
{
    TryTravelToRelay(0);
}

void ANoSignalPlayerPawn::SelectRelay2()
{
    TryTravelToRelay(1);
}

void ANoSignalPlayerPawn::SelectRelay3()
{
    TryTravelToRelay(2);
}

void ANoSignalPlayerPawn::SelectRelay4()
{
    TryTravelToRelay(3);
}

void ANoSignalPlayerPawn::TryTravelToRelay(const int32 RelayIndex)
{
    if (ANoSignalGameModeBase* GM = GetWorld() ? GetWorld()->GetAuthGameMode<ANoSignalGameModeBase>() : nullptr)
    {
        GM->TravelToRelay(RelayIndex);
    }
}

void ANoSignalPlayerPawn::PrintControlHints() const
{
    if (!GEngine)
    {
        return;
    }

    GEngine->AddOnScreenDebugMessage(-1, 8.0f, FColor(190, 190, 190), TEXT("Move: ZQSD + Mouse (AZERTY)"));
    GEngine->AddOnScreenDebugMessage(-1, 8.0f, FColor(190, 190, 190), TEXT("Actions: TAB Scan | E Repair Node | F Boost Core | R Rest | 1-4 Move Sector"));
}
