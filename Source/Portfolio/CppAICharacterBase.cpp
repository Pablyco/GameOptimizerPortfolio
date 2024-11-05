// Fill out your copyright notice in the Description page of Project Settings.


#include "CppAICharacterBase.h"

#include "Const.h"
#include "HealthBarWidget.h"
#include "NPC.h"
#include "PortfolioCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"

ACppAICharacterBase::ACppAICharacterBase() :
WidgetComponent{CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthValue"))},
Health{MaxHealth},
RightFistCollisionBox{CreateDefaultSubobject<UBoxComponent>(TEXT("RightFistCollisionBox"))}
{
	PrimaryActorTick.bCanEverTick = true;

	if (WidgetComponent)
	{
		WidgetComponent->SetupAttachment(RootComponent);
		WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
		WidgetComponent->SetRelativeLocation(defs::HealthBarZ);
		static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClass{TEXT("/Game/UI/WBP_HealthBar")};
		if(WidgetClass.Succeeded())
		{
			WidgetComponent->SetWidgetClass(WidgetClass.Class);
		}
	}
	if (RightFistCollisionBox)
	{
		RightFistCollisionBox->SetBoxExtent(defs::RightFistBoxSize,false);
		FAttachmentTransformRules const Rules{
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::KeepWorld,
			false
		};
		RightFistCollisionBox->AttachToComponent(GetMesh(),Rules,defs::RightFistSocketName);
		RightFistCollisionBox->SetRelativeLocation(defs::CollisionBoxLocation);
		
	}

}

void ACppAICharacterBase::BeginPlay()
{
	Super::BeginPlay();
	RightFistCollisionBox->OnComponentBeginOverlap.AddDynamic(this,&ACppAICharacterBase::OnAttackOverapBegin);
	RightFistCollisionBox->OnComponentEndOverlap.AddDynamic(this,&ACppAICharacterBase::OnAttackOverapEnd);
}

void ACppAICharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (auto const widget = Cast<UHealthBarWidget>(WidgetComponent->GetUserWidgetObject()))
	{
		widget->SetBarValuePercent(Health / MaxHealth);
		
	}

}

void ACppAICharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float ACppAICharacterBase::GetHealth() const
{
	return Health;
}

float ACppAICharacterBase::GetMaxHealth() const
{
	return MaxHealth;
}

void ACppAICharacterBase::SetHealth(float const NewHealth)
{
	Health = NewHealth;
	if (Cast<APortfolioCharacter>(this))
	{
		if (Health <= 0)
		{
			UE_LOG(LogTemp,Error,TEXT("YOU LOSE"));
			GetWorld()->GetFirstPlayerController()->ConsoleCommand("quit");
		}
	}
	else if (Cast<ANPC>(this))
	{
		if (Health <= 0)
		{
			UE_LOG(LogTemp,Error,TEXT("YOU WIN"));
			GetWorld()->GetFirstPlayerController()->ConsoleCommand("quit");
		}
	}
}

void ACppAICharacterBase::AttackStart()
{
	RightFistCollisionBox->SetCollisionProfileName("Fist");
	RightFistCollisionBox->SetNotifyRigidBodyCollision(true);
}

void ACppAICharacterBase::AttackEnd()
{
	RightFistCollisionBox->SetCollisionProfileName("Fist");
	RightFistCollisionBox->SetNotifyRigidBodyCollision(false);
}

void ACppAICharacterBase::OnAttackOverapBegin(UPrimitiveComponent* const OverlappedComponent, AActor* const OtherActor,
	UPrimitiveComponent* const OtherComponent, int const OtherBodyIndex, bool const FromSweep,
	FHitResult const& SweepResult)
{
	if (OtherActor == this)
	{
		return;
	}
	if (auto const Enemy = Cast<ANPC>(OtherActor))
	{
		auto const NewHealth = Enemy->GetHealth() - Enemy->GetMaxHealth() * 0.1f;
		Enemy->SetHealth(NewHealth);
	}
	else if (auto const Player = Cast<APortfolioCharacter>(OtherActor))
	{
		auto const NewHealth = Player->GetHealth() - Player->GetMaxHealth() * 0.07f;
		Player->SetHealth(NewHealth);
	}
}

void ACppAICharacterBase::OnAttackOverapEnd(UPrimitiveComponent* const OverlappedComponent, AActor* const OtherActor,
	UPrimitiveComponent* const OtherComponent, int const OtherBodyIndex)
{
	
}

