// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_IncrementPathIndex.h"

#include "NPC.h"
#include "NPC_AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_IncrementPathIndex::UBTTask_IncrementPathIndex(FObjectInitializer const& ObjectInitializer) :UBTTask_BlackboardBase{ObjectInitializer}
{
	NodeName = TEXT("Increment Path Index");
}

EBTNodeResult::Type UBTTask_IncrementPathIndex::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// try and get the AI Controller
	if (auto* const controller = Cast<ANPC_AIController>(OwnerComp.GetAIOwner()))
	{
		//try and get NPC
		if (auto* const NPC = Cast<ANPC>(controller->GetPawn()))
		{
			//try and get the blackboard
			if(auto* const bc = OwnerComp.GetBlackboardComponent())
			{
				auto const NoOfPoints = NPC->GetPatrolPath()->Num();
				auto const MinIndex = 0;
				auto const MaxIndex = NoOfPoints - 1;
				auto Index = bc->GetValueAsInt(GetSelectedBlackboardKey());

				//change direction if we are at the first of last index if we are in biderectional mode
				if(bBiDirectional)
				{
					if (Index >= MaxIndex && Direction == EDirectionType::Forward)
					{
						Direction = EDirectionType::Reverse;
					}
					else if (Index == MinIndex && Direction == EDirectionType::Reverse)
					{
						Direction = EDirectionType::Forward;
					}
				}
				// write new value of index to blackboard
				bc->SetValueAsInt(GetSelectedBlackboardKey(),
					Direction == EDirectionType::Forward
					? (++Index % NoOfPoints)  // Apply modulo to forward direction
					: (--Index + NoOfPoints) % NoOfPoints);  // Handle negative numbers in backward direction

				//finish with success
				FinishLatentTask(OwnerComp,EBTNodeResult::Succeeded);
				return EBTNodeResult::Succeeded;
			}

			
		}
	}
	return EBTNodeResult::Failed;
}
