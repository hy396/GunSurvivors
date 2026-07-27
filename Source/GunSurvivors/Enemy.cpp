// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Kismet/GameplayStatics.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	SetRootComponent(CapsuleComp);

	EnemyFlipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("EnemyFlipbook"));
	EnemyFlipbook->SetupAttachment(RootComponent);
	
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//怪物追击玩家，帧调用
	if(bIsAlive && bCanFollow && Player)
	{
		FVector CurrentLocation = GetActorLocation();
		FVector PlayerLocation = Player->GetActorLocation();

		FVector DirectionToPlayer = PlayerLocation - CurrentLocation;
		float DistanceToPlayer = DirectionToPlayer.Length();

		if(DistanceToPlayer >= StopDistance)
		{
			DirectionToPlayer.Normalize();//归一化
			FVector NewLocation = CurrentLocation + (DirectionToPlayer * MovementSpeed * DeltaTime);
			SetActorLocation(NewLocation);
		}
		
		//把脸朝向角色
		CurrentLocation = GetActorLocation();
		float FlipbookXScale = EnemyFlipbook->GetComponentRotation().Yaw;

		if ((PlayerLocation.X - CurrentLocation.X) >= 0.0f) // Player is on the right side of the enemy
		{
			if (FlipbookXScale == 180.0f)
			{
				EnemyFlipbook->SetWorldRotation(FRotator(0.0f,0.0f,0.0f));
			}
		}
		else // Player is on the left side of the enemy
		{
			if (FlipbookXScale == 0.0f)
			{
				EnemyFlipbook->SetWorldRotation(FRotator(0.0f,180.0f,0.0f));
			}
		}
	}
}

void AEnemy::Die()
{
	if(!bIsAlive) return;
	
	bIsAlive = false;
	bCanFollow = false;

	EnemyFlipbook->SetFlipbook(DeadFlipbookAsset);
	EnemyFlipbook->SetTranslucentSortPriority(-5);

	//死亡内的广播委托
	EnemyDiedDelegate.Broadcast();
	
	UGameplayStatics::PlaySound2D(GetWorld(), DieSound);
	
	float DestroyTime = 10.0f;
	GetWorldTimerManager().SetTimer(DestroyTimer, this, &AEnemy::OnDestroyTimerTimeout, 1.0f, false, DestroyTime);
}

void AEnemy::OnDestroyTimerTimeout()
{
	Destroy();
}

