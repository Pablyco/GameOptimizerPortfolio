// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PatrolPath.h"
#include "BehaviorTree/BehaviorTree.h"
#include "CombatInterface.h"
#include "CppAICharacterBase.h"
#include "NPC.generated.h"

UCLASS()
class PORTFOLIO_API ANPC : public ACppAICharacterBase,public ICombatInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANPC();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UBehaviorTree* GetBehaviorTree() const;

	APatrolPath* GetPatrolPath() const;

	UAnimMontage* GetMontage() const;

	int MeleeAttack_Implementation() override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


private:
	//We set a behavior tree in the editor
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category= "AI", meta=(AllowPrivateAccess= "true"))
	UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category= "AI", meta=(AllowPrivateAccess= "true"))
	APatrolPath* PatrolPath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category= "Animation", meta=(AllowPrivateAccess= "true"))
	UAnimMontage* Montage;
};
