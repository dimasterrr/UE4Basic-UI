// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TankPawn.h"
#include "PlayerTankPawn.generated.h"

class UInventoryEquipComponent;
class UCameraComponent;
class USpringArmComponent;
class UInventoryComponent;
class UInventoryManagerComponent;

UCLASS()
class DESTRUCTIVEFORCE_API APlayerTankPawn : public ATankPawn
{
	GENERATED_BODY()

private:
	float ActiveForwardAxis = 0.f;
	float ActiveRightAxis = 0.f;
	float CurrentRightRate = 0.f;

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UCameraComponent* Camera;

	// #START: Inventory
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UInventoryComponent* InventoryComponent;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UInventoryEquipComponent* InventoryEquipComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UInventoryManagerComponent* InventoryManagerComponent;
	// #END: Inventor

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Speed")
	float RotationSpeed = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement|Speed")
	float RotationInterpolationSpeed = .4f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Effects")
	TSubclassOf<UCameraShakeBase> DamageCameraShakeTemplate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Effects")
	float DamageCameraShakeRate;

private:
	void PerformMove(float DeltaTime);
	void PerformRotate(float DeltaTime);

	UStaticMeshComponent* GetEquipComponent(EItemEquipSlot Slot);
	
protected:
	virtual void BeginPlay() override;
	
public:
	APlayerTankPawn();
	virtual void Tick(float DeltaTime) override;
	void OnMoveForward(const float Value);
	void OnTurnRight(const float Value);

	virtual bool TakeDamage(const FDamageData& Data) override;
	virtual void OnDieEvent() override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnDieUi();

	virtual void PossessedBy(AController* NewController) override;

	UInventoryManagerComponent* GetInventoryManager();

	virtual void EquipItem(EItemEquipSlot Slot, FName ItemID) override;
	virtual void UnEquipItem(EItemEquipSlot Slot, FName ItemID) override;
};
