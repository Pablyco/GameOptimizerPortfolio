// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_isPlayerInMeleeRange.h"

#include "NPC.h"
#include "NPC_AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UBTService_isPlayerInMeleeRange::UBTService_isPlayerInMeleeRange()
{
	bNotifyBecomeRelevant = true;
	NodeName = TEXT("Is Player In Melee Range");
}

void UBTService_isPlayerInMeleeRange::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// get controller and NPC
	auto const * const Controller = Cast<ANPC_AIController>(OwnerComp.GetAIOwner());
	auto const * const NPC = Cast<ANPC>(Controller->GetPawn());

	//get player character
	auto const * const Player = UGameplayStatics::GetPlayerCharacter(GetWorld(),0);

	//write true or false to the blackboard key depending on whether or not the player is in range
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(GetSelectedBlackboardKey(),NPC->GetDistanceTo(Player)<= MeleeRange);
}
