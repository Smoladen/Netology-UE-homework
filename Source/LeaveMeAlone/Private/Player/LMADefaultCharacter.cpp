// LeaveMeAlone Game by Netologya. All rights Reserved


#include "Player/LMADefaultCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/DecalComponent.h"
#include "Components/InputComponent.h"
#include "Components/LMAHealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Weapon/LMAWeaponComponent.h"


ALMADefaultCharacter::ALMADefaultCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->SetUsingAbsoluteRotation(true);
	SpringArmComponent->TargetArmLength = Armlength;
	SpringArmComponent->SetRelativeRotation(FRotator(YRotation, 0.0f, 0.0f));
	SpringArmComponent->bDoCollisionTest = false;
	SpringArmComponent->bEnableCameraLag = true;


	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->SetFieldOfView(FOV);
	CameraComponent->bUsePawnControlRotation = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	HealthComponent = CreateDefaultSubobject<ULMAHealthComponent>("HealthComponent");
	WeaponComponent = CreateDefaultSubobject<ULMAWeaponComponent>("Weapon");
}

void ALMADefaultCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (CursorMaterial) {
		CurrentCursor = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), CursorMaterial, CursorSize,FVector(0));
	}
	//new
	OnHealthChanged(HealthComponent->GetHealth());
	HealthComponent->OnHealthChanged.AddUObject(this, &ALMADefaultCharacter::OnHealthChanged);

	HealthComponent->OnDeath.AddUObject(this, &ALMADefaultCharacter::OnDeath);
}

void ALMADefaultCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!(HealthComponent->IsDead())) {
		RotationPlayerOnCursor();
	}

	UpdateStamina();
}

void ALMADefaultCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ALMADefaultCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ALMADefaultCharacter::MoveRight);
	PlayerInputComponent->BindAxis("ZoomCamera", this, &ALMADefaultCharacter::AdjustCameraZoom);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ALMADefaultCharacter::StartSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ALMADefaultCharacter::StopSprint);
}

void ALMADefaultCharacter::MoveForward(float Value) {
	AddMovementInput(GetActorForwardVector(), Value);
}

void ALMADefaultCharacter::MoveRight(float Value) {
	AddMovementInput(GetActorRightVector(), Value);
}

void ALMADefaultCharacter::StartSprint()
{
	
	if (Stamina > MinStaminaToRun)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		isSprinting = true;
	}
}

void ALMADefaultCharacter::StopSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = defaultWalkSpeed;
	isSprinting = false;
}

void ALMADefaultCharacter::UpdateStamina()
{
	if (isSprinting)
	{
	
		Stamina -= StaminaDrain;

		if (Stamina <= 0.0f)
		{
			Stamina = 0.0f;
			StopSprint();
		}
	}
	else
	{
		
		Stamina += StaminaRefill;
		if (Stamina > MaxStamina)
		{
			Stamina = MaxStamina;
		}
	}


	//HasStamina = Stamina > MinStaminaToRun;
}
void ALMADefaultCharacter::AdjustCameraZoom(float Value) {
	if (!SpringArmComponent)
		return;

	float NewArmLength = SpringArmComponent->TargetArmLength - Value * ZoomSpeed;

	SpringArmComponent->TargetArmLength = FMath::Clamp(NewArmLength, MinArmLength, MaxArmLength);
}
void ALMADefaultCharacter::OnDeath()
{
	CurrentCursor->DestroyRenderState_Concurrent();
	
	PlayAnimMontage(DeathMontage);
	
	GetCharacterMovement()->DisableMovement();
	
	SetLifeSpan(5.0f);

	if (Controller) {
		Controller->ChangeState(NAME_Spectating);
	}
}
void ALMADefaultCharacter::RotationPlayerOnCursor() {
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC) {
		FHitResult ResultHit;
		PC->GetHitResultUnderCursor(ECC_GameTraceChannel1, true, ResultHit);
		float FindRotatorResultYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ResultHit.Location).Yaw;
		SetActorRotation(FQuat(FRotator(0.0f, FindRotatorResultYaw, 0.0f)));
		if (CurrentCursor) {
			CurrentCursor->SetWorldLocation(ResultHit.Location);
		}
	}
}

//new
void ALMADefaultCharacter::OnHealthChanged(float NewHealth)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Health = %f"),
		NewHealth));
}