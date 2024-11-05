// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindPathPoint.h"

#include "NPC.h"
#include "NPC_AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_FindPathPoint::UBTTask_FindPathPoint(FObjectInitializer const& ObjectInitializer) :
UBTTask_BlackboardBase{ObjectInitializer}
{
	NodeName = TEXT("Find Path Point");
}

EBTNodeResult::Type UBTTask_FindPathPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//try to get the NPC's Controller
	if (auto* const controller = Cast<ANPC_AIController>(OwnerComp.GetAIOwner()))
	{
		//try to get blackboard component from the behaviour tree
		if (auto* const bc = OwnerComp.GetBlackboardComponent())
		{
			//get the current patrol path index from the blackboard
			auto const Index = bc->GetValueAsInt(GetSelectedBlackboardKey());

			//try to get the NPC
			if(auto* NPC = Cast<ANPC>(controller->GetPawn()))
			{
				//get the current patrol path vector from the NPC - This is local to the patrol path actor object
				auto const Point = NPC->GetPatrolPath()->GetPatrolPoints(Index);

				//convert to local vector to a global point
				auto const GlobalPoint = NPC->GetPatrolPath()->GetActorTransform().TransformPosition(Point);

				bc->SetValueAsVector(PatrolPathVectorKey.SelectedKeyName,GlobalPoint);

				//finish with success
				FinishLatentTask(OwnerComp,EBTNodeResult::Succeeded);
				return EBTNodeResult::Succeeded;
			}
		}
	}
	
	return EBTNodeResult::Failed;
}
