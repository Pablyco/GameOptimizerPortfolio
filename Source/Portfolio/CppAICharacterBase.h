// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CppAICharacterBase.generated.h"

UCLASS()
class PORTFOLIO_API ACppAICharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ACppAICharacterBase();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	float GetHealth() const;
	float GetMaxHealth()const;
	void SetHealth(float const NewHealth);
	void AttackStart();
	void AttackEnd();

private:
	UPROPERTY()
	class UWidgetComponent* WidgetComponent;
	float const MaxHealth{100.f};
	float Health;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Collision, meta=(AllowPrivateAccess = "true"))
	class UBoxComponent* RightFistCollisionBox;

	UFUNCTION()
	void OnAttackOverapBegin(
	UPrimitiveComponent* const OverlappedComponent,
	AActor* const OtherActor,
	UPrimitiveComponent* const OtherComponent,
	int const OtherBodyIndex,
	bool const FromSweep,
	FHitResult const& SweepResult
	);

	UFUNCTION()
	void OnAttackOverapEnd(
	UPrimitiveComponent* const OverlappedComponent,
	AActor* const OtherActor,
	UPrimitiveComponent* const OtherComponent,
	int const OtherBodyIndex
	);
};
