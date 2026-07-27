// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"

#include "Enemy.h"

ABullet::ABullet()
{
	PrimaryActorTick.bCanEverTick = true;
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SetRootComponent(SphereComp);

	BulletSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("BulletSprite"));
	BulletSprite->SetupAttachment(RootComponent);

	MovementDirection = FVector2D(1.0f, 0.0f);
}

void ABullet::BeginPlay()
{
	Super::BeginPlay();
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ABullet::OverlapBegin);
}

void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(bIsLaunched)
	{
		FVector2D DistanceToMove = MovementDirection * MovementSpeed * DeltaTime;

		FVector CurrentLocation = GetActorLocation();
		FVector NewLocation = CurrentLocation + FVector(DistanceToMove.X, 0.0f, DistanceToMove.Y);

		SetActorLocation(NewLocation);
	}
}

void ABullet::Launch(FVector2D Direction, float Speed)
{
	if (bIsLaunched) return;

	bIsLaunched = true;

	MovementDirection = Direction;
	MovementSpeed = Speed;

	float DeleteTime = 10.0f;
	GetWorldTimerManager().SetTimer(DeleteTimer, this, &ABullet::OnDeleteTimerTimeout, 1.0f, false, DeleteTime);
}

void ABullet::OnDeleteTimerTimeout()
{
	Destroy();
}

void ABullet::OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AEnemy *Enemy = Cast<AEnemy>(OtherActor);
	if(Enemy && Enemy->bIsAlive)
	{
		DisableBullet();
		Enemy->Die();
	}
}

void ABullet::DisableBullet()
{
	if(bIsDisabled) return;
	bIsDisabled = true;
	SphereComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);//关闭碰撞

	BulletSprite->DestroyComponent();//消除精灵组件
}

