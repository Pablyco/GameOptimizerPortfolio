// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"



// Sets default values
AShooterCharacter::AShooterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->GravityScale = 2.2f;
	GetCharacterMovement()->MaxAcceleration = 5000.f;
	GetCharacterMovement()->GroundFriction = 4.f;
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->MaxWalkSpeed = 1000.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 100.f;
	// Set default acceleration values
	GroundAcceleration = 5000.0f; // Tune this to adjust ground acceleration
	AirAcceleration = 50.0f;      // Lower value for air control

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(RootComponent); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = true; // Camera does not rotate relative to arm
	FollowCamera->FieldOfView = 105.f;
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

}

void AShooterCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	FVector ForwardDir = GetActorForwardVector();
	FVector RightDir = GetActorRightVector();

	float ForwardValue = GetInputAxisValue("MoveForward");
	float RightValue = GetInputAxisValue("MoveRight");

	FVector WishDir = ForwardDir * ForwardValue + RightDir * RightValue;
	float WishSpeed = WishDir.Size();  // Calculate desired speed based on input
	WishDir.Normalize();  // Convert WishDir to a unit vector

	if (GetCharacterMovement()->IsMovingOnGround())
	{
		Accelerate(WishDir, WishSpeed, GroundAcceleration);
	}
	else
	{
		AirAccelerate(WishDir, WishSpeed, AirAcceleration);
	}
}

void AShooterCharacter::Accelerate(FVector WishDir, float WishSpeed, float Acceleration)
{
	float CurrentSpeed = FVector::DotProduct(GetVelocity(), WishDir);
	float AddSpeed = WishSpeed - CurrentSpeed;

	if (AddSpeed <= 0.0f)
		return;

	float AccelSpeed = Acceleration * GetWorld()->GetDeltaSeconds() * WishSpeed;
	AccelSpeed = FMath::Min(AccelSpeed, AddSpeed);

	// Modify character's velocity directly
	FVector NewVelocity = GetVelocity() + AccelSpeed * WishDir;
	GetCharacterMovement()->Velocity = NewVelocity;
}

void AShooterCharacter::AirAccelerate(FVector WishDir, float WishSpeed, float Acceleration)
{
	const float MaxAirSpeed = 30.0f;  // Limit air speed similar to Quake
	WishSpeed = FMath::Min(WishSpeed, MaxAirSpeed);

	float CurrentSpeed = FVector::DotProduct(GetVelocity(), WishDir);
	float AddSpeed = WishSpeed - CurrentSpeed;

	if (AddSpeed <= 0.0f)
		return;

	float AccelSpeed = Acceleration * WishSpeed * GetWorld()->GetDeltaSeconds();
	AccelSpeed = FMath::Min(AccelSpeed, AddSpeed);

	// Modify character's velocity directly
	FVector NewVelocity = GetVelocity() + AccelSpeed * WishDir;
	GetCharacterMovement()->Velocity = NewVelocity;
}

void AShooterCharacter::StrafeAccelerate(FVector WishDir, float WishSpeed, float Acceleration)
{
	// Retrieve the current velocity from the CharacterMovement component
	FVector CurrentVelocity = GetCharacterMovement()->Velocity;

	// Calculate the current speed in the direction of WishDir
	float CurrentSpeedInDir = FVector::DotProduct(CurrentVelocity, WishDir);
	float AddSpeed = WishSpeed - CurrentSpeedInDir;

	// Only add speed if we’re below the desired speed in the WishDir direction
	if (AddSpeed <= 0.0f)
		return;

	// Calculate the amount of speed to add based on Acceleration
	float AccelSpeed = Acceleration * GetWorld()->GetDeltaSeconds() * WishSpeed;
	if (AccelSpeed > AddSpeed)
		AccelSpeed = AddSpeed;

	// Update the velocity by adding the new acceleration component to the existing velocity
	CurrentVelocity += AccelSpeed * WishDir;

	// Apply a max speed cap only on the horizontal (XY) speed
	float MaxSpeed = GetCharacterMovement()->MaxWalkSpeed;
	if (CurrentVelocity.Size2D() > MaxSpeed)
	{
		// Normalize the horizontal speed and scale it to the max allowed speed
		FVector HorizontalVelocity = CurrentVelocity.GetSafeNormal2D() * MaxSpeed;
		CurrentVelocity.X = HorizontalVelocity.X;
		CurrentVelocity.Y = HorizontalVelocity.Y;
	}

	// Preserve the vertical (Z) component for gravity and jumping effects
	CurrentVelocity.Z = GetCharacterMovement()->Velocity.Z;

	// Set the updated velocity back to the CharacterMovement component
	GetCharacterMovement()->Velocity = CurrentVelocity;
}

void AShooterCharacter::Jump()
{
	if (GetCharacterMovement()->IsMovingOnGround())
	{
		// Store current horizontal velocity before jumping
		FVector HorizontalVelocity = GetCharacterMovement()->Velocity;
		HorizontalVelocity.Z = 0; // Ignore vertical component

		// Call the jump function (default UE jump behavior)
		Super::Jump();

		// Reapply the stored horizontal velocity after jumping
		GetCharacterMovement()->Velocity = HorizontalVelocity + FVector(0, 0, GetCharacterMovement()->JumpZVelocity);
	}
}

void AShooterCharacter::Lean(float ScaleVal)
{
	const float MaxLeanAngle = 5.f;
	const float LeanSpeed = 5.f;

	//Check if the player moves, falls and moves to the right or left.
	if (GetVelocity().Length() > 0 && !GetCharacterMovement()->IsFalling() && ScaleVal != 0)
	{
		float CurrentRoll = GetControlRotation().Roll;
		float TargetRoll = 0.f;
        
		if (ScaleVal > 0)  // Leaning right
			{
			TargetRoll = MaxLeanAngle;
			}
		else if (ScaleVal < 0)  // Leaning left
			{
			TargetRoll = -MaxLeanAngle;
			}
        
		TargetRoll = FMath::Clamp(TargetRoll, -MaxLeanAngle, MaxLeanAngle);
        
		// Preserve current Pitch and Yaw, only modify Roll
		FRotator CurrentRot = GetControlRotation();
		FRotator TargetRot = FRotator(CurrentRot.Pitch, CurrentRot.Yaw, TargetRoll);
        
		GetController()->SetControlRotation(FMath::RInterpTo(
			GetControlRotation(), 
			TargetRot, 
			GetWorld()->DeltaTimeSeconds, 
			LeanSpeed
		));
	}
	else  // Return to center
		{
		// Preserve current Pitch and Yaw, only reset Roll
		FRotator CurrentRot = GetControlRotation();
		FRotator TargetRot = FRotator(CurrentRot.Pitch, CurrentRot.Yaw, 0);
        
		GetController()->SetControlRotation(FMath::RInterpTo(
			GetControlRotation(), 
			TargetRot, 
			GetWorld()->DeltaTimeSeconds, 
			LeanSpeed
		));
		}
}
void AShooterCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		FVector Forward = GetActorForwardVector();
		FVector Right = GetActorRightVector();

		FVector WishDir = (Forward * MovementVector.Y + Right * MovementVector.X).GetSafeNormal();

		float WishSpeed = GetCharacterMovement()->MaxWalkSpeed;

		if (!GetCharacterMovement()->IsMovingOnGround()) 
		{
			// If in air, use strafe acceleration
			StrafeAccelerate(WishDir, WishSpeed, AirAcceleration);
		}
		else
		{
			// add movement 
			AddMovementInput(ForwardDirection, MovementVector.Y);
			AddMovementInput(RightDirection, MovementVector.X);
		}
			
		Lean(MovementVector.X);
		
	}
}

void AShooterCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AShooterCharacter::Attack(const FInputActionValue& Value)
{
}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AShooterCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AShooterCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AShooterCharacter::Look);

		//Attack
		EnhancedInputComponent->BindAction(AttackAction,ETriggerEvent::Triggered, this,&AShooterCharacter::Attack);
	}
}

