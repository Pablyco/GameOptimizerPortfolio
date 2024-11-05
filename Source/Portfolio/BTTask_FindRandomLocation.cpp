// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindRandomLocation.h"

#include "NavigationSystem.h"
#include "NPC_AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_FindRandomLocation::UBTTask_FindRandomLocation(FObjectInitializer const& ObjectInitializer) :
UBTTask_BlackboardBase{ObjectInitializer}
{
	NodeName = "Find Random Location";
}

EBTNodeResult::Type UBTTask_FindRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//Get AI Controller + NPC:
	if (ANPC_AIController* const Controller = Cast<ANPC_AIController>(OwnerComp.GetAIOwner()))
	{
		//                 ^^^^^
		//                 This const means the pointer itself cannot be reassigned

		if (auto* const npc = Controller->GetPawn())
		{
			//obtain npc location to use as starting point:
			auto const StartPos = npc->GetActorLocation();

			// now get the navigation system and generate a random location:
			if (auto* const NavSystem = UNavigationSystemV1::GetCurrent(GetWorld()))
			{
				FNavLocation NavLocation;
				if (NavSystem->GetRandomPointInNavigableRadius(StartPos,SearchRadius,NavLocation))
				{
					OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(),NavLocation.Location);
				}

				//finish the task with success
				FinishLatentTask(OwnerComp,EBTNodeResult::Succeeded);
				return EBTNodeResult::Succeeded;
			}
		}
	}
	return EBTNodeResult::Failed;
}

