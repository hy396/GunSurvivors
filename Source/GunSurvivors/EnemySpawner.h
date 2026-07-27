// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Engine/TimerHandle.h"

#include "Enemy.h"
#include "TopdownCharacter.h"
#include "GunSurvivorsGameMode.h"

#include "EnemySpawner.generated.h"

UCLASS()
class GUNSURVIVORS_API AEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AEnemy> EnemyActorToSpawn;

	//是否调用生怪
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanNewAI = true;
	
	//生成的时间间隔
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnTime = 1.0f;

	//难度上升角色加强射速
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ShootSpeed = 0.05f;

	//最小射速内置cd
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinShootSpeed = 0.25f;

	//初始速度
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EnemySpeed = 50.0f;

	//最高移动速度
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxEnemySpeed = 80.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float NanduUpSpeed = 2.5f;
	
	//圆的半径
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnDistance = 400.0f;

	//生成敌人的和
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int TotalEnemyCount = 0;

	//达到数量可提升难度的数量
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int DifficultySpawnInterval = 10;

	//生成时间间隔的最小值
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnTimeMinimumLimit = 0.4f;

	//难度提升时降低的生成时间间隔的大小
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DecreaseSpawnTimerByEveryInterval = 0.05f;

	ATopdownCharacter* Player;

	FTimerHandle SpawnTimer;

	AGunSurvivorsGameMode* MyGameMode;
	
	AEnemySpawner();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	
	void OnSpawnTimerTimeout();
	//开始生成
	void StartSpawning();
	//停止生成
	void StopSpawning();
	//生成敌人
	void SpawnEnemy();

	//替代Enemy中的BeginPlay，减少每一位Enemy在beginPlay上多余的开销
	void SetupEnemy(AEnemy* Enemy);

	UFUNCTION()
	void OnEnemyDied();

	UFUNCTION()
	void OnPlayerDied();

	UFUNCTION(BlueprintImplementableEvent)
	void NewWbp();

	UFUNCTION(BlueprintCallable)
	void GameStop();

	UFUNCTION(BlueprintCallable)
	void GameStart();
};
