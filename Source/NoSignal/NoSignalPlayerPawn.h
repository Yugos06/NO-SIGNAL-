#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "NoSignalPlayerPawn.generated.h"

class UCameraComponent;
class UFloatingPawnMovement;
class USceneComponent;
class USpringArmComponent;
class UStaticMeshComponent;

UCLASS()
class NOSIGNAL_API ANoSignalPlayerPawn : public APawn
{
    GENERATED_BODY()

public:
    ANoSignalPlayerPawn();

    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USceneComponent> Root;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UStaticMeshComponent> BodyMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USpringArmComponent> SpringArm;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UCameraComponent> Camera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UFloatingPawnMovement> Movement;

    void MoveForward(float Value);
    void MoveRight(float Value);
    void TurnYaw(float Value);
    void LookUpPitch(float Value);

    void HandleScan();
    void HandleRepair();
    void HandleBoostHub();
    void HandleRest();

    void SelectRelay1();
    void SelectRelay2();
    void SelectRelay3();
    void SelectRelay4();
    void TryTravelToRelay(int32 RelayIndex);
    void PrintControlHints() const;
};
