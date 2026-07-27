// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawner.h"

#include <basetyps.h>

#include "Kismet/GameplayStatics.h"

AEnemySpawner::AEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	AGameModeBase *GameMode = UGameplayStatics::GetGameMode(GetWorld());
	if(GameMode)
	{
		MyGameMode = Cast<AGunSurvivorsGameMode>(GameMode);
		check(MyGameMode);
	}
	
	AActor *PlayerActor = UGameplayStatics::GetActorOfClass(GetWorld(),ATopdownCharacter::StaticClass());
	if(PlayerActor)
	{
		Player = Cast<ATopdownCharacter>(PlayerActor);
		//玩家死亡的委托
		Player->PlayerDiedDelegate.AddDynamic(this,&AEnemySpawner::OnPlayerDied);
		//bCanFollow = true;
	}
	if(bCanNewAI)
	{
		StartSpawning();
	}
}

void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemySpawner::OnSpawnTimerTimeout()
{
	SpawnEnemy();
}

void AEnemySpawner::StartSpawning()
{
	GetWorldTimerManager().SetTimer(SpawnTimer, this, &AEnemySpawner::OnSpawnTimerTimeout, SpawnTime, true, SpawnTime);

	// auto Lambda = [this]()
	// {
	// 	int ScoreToAdd = 10;
	// 	//死亡得分中调用GameMode加分，在GameMode中使用蓝图委托修改得分
	// 	MyGameMode->AddScore(ScoreToAdd);
	// };
	// FTimerDelegate QwQ;
	// QwQ.BindLambda(this,Lambda);
	// GetWorldTimerManager().SetTimer(SpawnTimer,Lambda, SpawnTime, true, SpawnTime);
	// GetWorldTimerManager().SetTimer(SpawnTimer,QwQ, SpawnTime, true, SpawnTime);
	
	// GetWorldTimerManager().SetTimer(SpawnTimer, [this]()
	// {
	// 	int ScoreToAdd = 10;
	// 	//死亡得分中调用GameMode加分，在GameMode中使用蓝图委托修改得分
	// 	MyGameMode->AddScore(ScoreToAdd);
	// }, SpawnTime, true, SpawnTime);
}

void AEnemySpawner::StopSpawning()
{
	GetWorldTimerManager().ClearTimer(SpawnTimer);
}

void AEnemySpawner::SpawnEnemy()
{
	//生成敌人
	FVector2D RandomPosition = FVector2D(FMath::VRand());
	RandomPosition.Normalize();//归一
	RandomPosition *= SpawnDistance;
	FVector EnemyLocation = GetActorLocation() + FVector(RandomPosition.X, 0.0f, RandomPosition.Y);

	AEnemy *Enemy = GetWorld()->SpawnActor<AEnemy>(EnemyActorToSpawn,EnemyLocation,FRotator::ZeroRotator);
	SetupEnemy(Enemy);
	//调试用
	//FString Message = FString::Printf(TEXT("AI移动速度：%f \n AI生成间隔：%f \n 玩家射速：%f"), Enemy->MovementSpeed,SpawnTime,Player->ShootCooldownDurationInSeconds);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, Message);
	
	//增加难度
	//生成一个敌人后count+1
	TotalEnemyCount += 1;
	if((TotalEnemyCount % DifficultySpawnInterval) == 0)
	{
		//每生成10个怪增加难度，减少生成时间
		if(SpawnTime > SpawnTimeMinimumLimit)
		{
			//减少敌人生成时间间隔
			SpawnTime -= DecreaseSpawnTimerByEveryInterval;
			if(SpawnTime < SpawnTimeMinimumLimit)
			{
				SpawnTime = SpawnTimeMinimumLimit;
			}
			//AI移速调整
			EnemySpeed += NanduUpSpeed;
			if(EnemySpeed > MaxEnemySpeed)
			{
				EnemySpeed = MaxEnemySpeed;
			}
			//射速调整
			Player->ShootCooldownDurationInSeconds -= ShootSpeed;
			if(Player->ShootCooldownDurationInSeconds < MinShootSpeed)
			{
				Player->ShootCooldownDurationInSeconds = MinShootSpeed;
			}
			//刷新一下怪生成间隔的调用函数
			StopSpawning();
			StartSpawning();
		}
	}
}

void AEnemySpawner::SetupEnemy(AEnemy* Enemy)
{
	if(Enemy)
	{
		Enemy->Player = Player;
		Enemy->bCanFollow = true;
		Enemy->MovementSpeed = EnemySpeed;
		//绑定委托函数
		Enemy->EnemyDiedDelegate.AddDynamic(this,&AEnemySpawner::OnEnemyDied);
	}
}

void AEnemySpawner::OnEnemyDied()
{
	//GEngine->AddOnScreenDebugMessage(-1,5.0f,FColor::Blue,TEXT("寄！"));
	int ScoreToAdd = 10;
	//死亡得分中调用GameMode加分，在GameMode中使用蓝图委托修改得分
	MyGameMode->AddScore(ScoreToAdd);
}

void AEnemySpawner::OnPlayerDied()
{
	StopSpawning();

	TArray<AActor*> EnemyArray;
	//找到场景所以敌人放入数组
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy::StaticClass(), EnemyArray);
	//遍历Enemy
	for (AActor *EnemyActor : EnemyArray)
	{
		//Cast转换
		AEnemy* Enemy = Cast<AEnemy>(EnemyActor);
		//判断是否为空和是否死了
		if (Enemy && Enemy->bIsAlive)
		{
			//追击玩家停止
			Enemy->bCanFollow = false;
		}
	}

	//重启
	//MyGameMode->RestartGame();
	NewWbp();
}

void AEnemySpawner::GameStop()
{
	Player->bCanMove = false;
	Player->bCanShoot = false;
	StopSpawning();
	TArray<AActor*> EnemyArray;
	//找到场景所以敌人放入数组
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy::StaticClass(), EnemyArray);
	//遍历Enemy
	for (AActor *EnemyActor : EnemyArray)
	{
		//Cast转换
		AEnemy* Enemy = Cast<AEnemy>(EnemyActor);
		//判断是否为空和是否死了
		if (Enemy && Enemy->bIsAlive)
		{
			//追击玩家停止
			Enemy->bCanFollow = false;
		}
	}
}

void AEnemySpawner::GameStart()
{
	Player->bCanMove = true;
	Player->bCanShoot = true;
	StartSpawning();
	TArray<AActor*> EnemyArray;
	//找到场景所以敌人放入数组
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy::StaticClass(), EnemyArray);
	//遍历Enemy
	for (AActor *EnemyActor : EnemyArray)
	{
		//Cast转换
		AEnemy* Enemy = Cast<AEnemy>(EnemyActor);
		//判断是否为空和是否死了
		if (Enemy && Enemy->bIsAlive)
		{
			//追击玩家停止
			Enemy->bCanFollow = true;
		}
	}
}

// class FSB
// {
// public:
// 	
// };
//
//
// class FVVV
// {
// public:
// 	int op;
// 	FVVV(){}
// 	FVVV(const FSB &InData){}
// 	FVVV operator =(const int &qwq)
// 	{
// 		return FVVV(qwq);
// 	}
// 	explicit FVVV(const int &fw)
// 	{
// 		op = fw;
// 	}
// };
// void qwq()
// {
// 	FVVV qwq;
// 	FSB sb;
// 	qwq = sb;
// 	int nm = 1;
// 	qwq = nm;
// 	FVVV op(nm);
// }