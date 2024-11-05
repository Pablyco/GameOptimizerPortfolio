// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC_AIController.h"

#include "NPC.h"
#include "PortfolioCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

ANPC_AIController::ANPC_AIController(FObjectInitializer const& ObjectInitializer)
{
	SetupPerceptionSystem();
}

void ANPC_AIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
//if InPawn is NPC get can obtain the behavior tree
	if (ANPC* const npc = Cast<ANPC>(InPawn))
	{
		if(UBehaviorTree* const tree = npc->GetBehaviorTree())
		{
			//now I set the blackboard and run the behavior tree:
			UBlackboardComponent* BlackboardComp;
			UseBlackboard(tree->BlackboardAsset,BlackboardComp);
			Blackboard = BlackboardComp;
			RunBehaviorTree(tree);
		}
	}
}

void ANPC_AIController::SetupPerceptionSystem()
{
	//Create Sight perception system
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	if (SightConfig)
	{
		//create a perception component - create default subobject and set as the current percetion component in the system
		SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component")));

		//if the player is whithin this radius then he will be seen
		SightConfig->SightRadius = 500.0f;
		//the radius where the npc stop seeing the player
		SightConfig->LoseSightRadius = SightConfig->SightRadius + 25.0f;
		//the npc only can only see in front
		SightConfig->PeripheralVisionAngleDegrees = 90.0f;
		//maxAge is the time after which perceived stimulus is forgotten
		SightConfig->SetMaxAge(5.0f);
		//allows NPC to continue to see stimulus source if reimains whithin the specified range
		SightConfig->AutoSuccessRangeFromLastSeenLocation = 520.0f;
		//DetectionByAffilations filter the types of stimulus source for this project can detect Enemies, Friendlies and Neutrals
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

		//set the dominant sense to sense by getting the sense implementation from sight configuration
		GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());
		//Set target detected to be called when something is sensed
		GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this,&ANPC_AIController::OnTargetDetected);
		//configure the sense system with the sight configure object
		GetPerceptionComponent()->ConfigureSense((*SightConfig));
	}
}

void ANPC_AIController::OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus)
{
	if (auto* const ch= Cast<APortfolioCharacter>(Actor))
	{
		GetBlackboardComponent()->SetValueAsBool("CanSeePlayer",Stimulus.WasSuccessfullySensed());
	}
}
