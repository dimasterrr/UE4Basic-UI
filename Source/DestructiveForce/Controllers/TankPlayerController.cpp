#include "TankPlayerController.h"

#include "Base/PlayerHUD.h"
#include "Inventory/InventoryMainWidget.h"
#include "Inventory/InventoryWidget.h"
#include "Kismet/KismetMathLibrary.h"
#include "Modules/Inventory/Components/InventoryManagerComponent.h"

ATankPlayerController::ATankPlayerController()
{
	bShowMouseCursor = true;
}

void ATankPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	PerformMousePosition(DeltaSeconds);
}

void ATankPlayerController::PerformMousePosition(const float DeltaSeconds)
{
	if (!PossessedPawn) return;
	if (PossessedPawn->IsDie()) return;

	FVector MousePosition, MouseDirection;
	DeprojectMousePositionToWorld(MousePosition, MouseDirection);

	const auto PawnPosition = PossessedPawn->GetActorLocation();

	auto Direction = MousePosition - PlayerCameraManager->GetCameraLocation();
	Direction.Normalize();

	auto CurrentMousePosition = MousePosition + Direction * 1000.f;
	CurrentMousePosition.Z = PawnPosition.Z;

	const auto CurrentLocation = PossessedPawn->GetActorLocation();
	const auto CurrentTurretRotation = PossessedPawn->GetActorRotation(); // TODO: Rotation turret

	auto TargetTurretRotation = UKismetMathLibrary::FindLookAtRotation(CurrentLocation, CurrentMousePosition);
	TargetTurretRotation.Pitch = CurrentTurretRotation.Pitch;
	TargetTurretRotation.Roll = CurrentTurretRotation.Roll;

	PossessedPawn->SetTurretRotation(TargetTurretRotation);
}

void ATankPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	PossessedPawn = Cast<APlayerTankPawn>(InPawn);
}

void ATankPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (!InputComponent) return;

	InputComponent->BindAxis("MoveForward", this, &ATankPlayerController::OnMoveForward);
	InputComponent->BindAxis("TurnRight", this, &ATankPlayerController::OnTurnRight);

	InputComponent->BindAction("Fire", IE_Pressed, this, &ATankPlayerController::OnFireStart);
	InputComponent->BindAction("Fire", IE_Released, this, &ATankPlayerController::OnFireStop);

	InputComponent->BindAction("FireSpecial", IE_Pressed, this, &ATankPlayerController::OnFireSpecialStart);
	InputComponent->BindAction("FireSpecial", IE_Released, this, &ATankPlayerController::OnFireSpecialStop);

	InputComponent->BindAction("Reload", IE_Pressed, this, &ATankPlayerController::OnReload);
	InputComponent->BindAction("SwitchWeapon", IE_Pressed, this, &ATankPlayerController::OnSwitchWeapon);

	InputComponent->BindAction("OpenInventory", IE_Pressed, this, &ATankPlayerController::OnInventoryClicked);
}

void ATankPlayerController::OnMoveForward(float Value)
{
	PossessedPawn->OnMoveForward(Value);
}

void ATankPlayerController::OnTurnRight(float Value)
{
	PossessedPawn->OnTurnRight(Value);
}

void ATankPlayerController::OnFireStart()
{
	PossessedPawn->OnFireStart();
}

void ATankPlayerController::OnFireStop()
{
	PossessedPawn->OnFireStop();
	OnLeftMouseButtonUp.Broadcast();
}

void ATankPlayerController::OnFireSpecialStart()
{
	PossessedPawn->OnFireSpecialStart();
}

void ATankPlayerController::OnFireSpecialStop()
{
	PossessedPawn->OnFireSpecialStop();
}

void ATankPlayerController::OnReload()
{
	PossessedPawn->OnReload();
}

void ATankPlayerController::OnSwitchWeapon()
{
	PossessedPawn->OnSwitchWeapon();
}

void ATankPlayerController::OnInventoryClicked()
{
	const auto Hud = Cast<APlayerHUD>(GetHUD());
	if (!Hud) return;

	const auto WidgetRaw = Hud->ShowWidget(EWidgetID::Inventory, 10);
	const auto Widget = Cast<UInventoryMainWidget>(WidgetRaw);
	if (!Widget) return;

	const auto InventoryManager = PossessedPawn->GetInventoryManager();
	if (!InventoryManager) return;

	InventoryManager->PrepareInventoryWidget(Widget);

	// Set input mode to UI
	FInputModeUIOnly Property;
	Property.SetWidgetToFocus(Widget->TakeWidget());
	Property.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
	SetInputMode(Property);
}
