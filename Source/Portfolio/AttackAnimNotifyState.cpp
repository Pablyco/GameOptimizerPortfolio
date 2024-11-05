// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackAnimNotifyState.h"

#include "CppAICharacterBase.h"

void UAttackAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                         float TotalDuration)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		if(ACppAICharacterBase* const character = Cast<ACppAICharacterBase>(MeshComp->GetOwner()))
		{
			character->AttackStart();
		}
	}
}

void UAttackAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		if(ACppAICharacterBase* const character = Cast<ACppAICharacterBase>(MeshComp->GetOwner()))
		{
			character->AttackEnd();
		}
	}
}
