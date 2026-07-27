// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Engine/TimerHandle.h"

#include "Enemy.h"
#include "TopdownCharacter.h"
#include "GunSurvivorsGameMode.h"

#include "EnemySpawner.generated.h"

/**
 * AEnemySpawner - 敌人生成器 + 难度曲线驱动器
 *
 * 整个游戏最核心的调度 Actor:
 *   1. 周期性 spawn AEnemy(在以自身为圆心、SpawnDistance 为半径的圆上随机取点)
 *   2. 每个新敌人 SetupEnemy(Player / bCanFollow / MovementSpeed + 绑定 EnemyDiedDelegate)
 *   3. 监听玩家/敌人委托,处理得分与游戏结束
 *   4. 难度曲线:每生成 DifficultySpawnInterval(默认 10)只敌人自动升一档难度
 *
 * 难度曲线三参数同步调整(在 SpawnEnemy 末尾触发):
 *   - SpawnTime  ↓ DecreaseSpawnTimerByEveryInterval(下限 SpawnTimeMinimumLimit)
 *   - EnemySpeed ↑ NanduUpSpeed(上限 MaxEnemySpeed)
 *   - Player->ShootCooldownDurationInSeconds ↓ ShootSpeed(下限 MinShootSpeed)
 * 注意:调整 SpawnTime 后必须 StopSpawning() + StartSpawning() 重启定时器,
 * 因为 SetTimer 第一次延迟参数(initialDelay)在 SetTimer 调用时已确定。
 *
 * 委托监听:
 *   - Player->PlayerDiedDelegate   → OnPlayerDied(停止生成 + 关闭所有敌人追击 + NewWbp 重启 UI)
 *   - Enemy->EnemyDiedDelegate(每只)→ OnEnemyDied(给 GameMode 加 10 分)
 *
 * GameStop / GameStart:
 *   蓝图可调,实现"暂停"语义:关闭玩家输入与所有敌人追击但不清空敌人列表。
 *
 * @see AEnemy                  被生成的对象
 * @see ATopdownCharacter       监听 PlayerDiedDelegate,游戏加速目标
 * @see AGunSurvivorsGameMode   接收 AddScore
 */
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

	//缓存的玩家引用(BeginPlay 中查询)
	ATopdownCharacter* Player;

	//循环生成 Timer(SpawnTime 秒一次)
	FTimerHandle SpawnTimer;

	//缓存的 GameMode(BeginPlay 中查询)
	AGunSurvivorsGameMode* MyGameMode;

	/** 构造函数(空) */
	AEnemySpawner();

	/** 初始化:获取 GameMode + Player + 绑定 PlayerDiedDelegate + 按 bCanNewAI 启动生成 */
	virtual void BeginPlay() override;

	/** 每帧(空) */
	virtual void Tick(float DeltaTime) override;

	/** SpawnTimer 循环回调:每次调用 SpawnEnemy 生成一个敌人 */
	void OnSpawnTimerTimeout();

	/** 启动循环生成:以 SpawnTime 为间隔调用 OnSpawnTimerTimeout(adjust 后必须重启) */
	void StartSpawning();

	/** 停止循环生成:ClearTimer(SpawnTimer) */
	void StopSpawning();

	/** 生成一只敌人:圆周随机点 + SpawnActor + SetupEnemy + TotalEnemyCount++ + 每 N 只提升难度 */
	void SpawnEnemy();

	/** 替代 AEnemy::BeginPlay 的初始化(减少每只 Enemy 的 BeginPlay 开销):
	 *  设置 Player / bCanFollow=true / MovementSpeed + 绑定 EnemyDiedDelegate */
	void SetupEnemy(AEnemy* Enemy);

	/** 敌人死亡回调:每只敌人死亡时 MyGameMode->AddScore(10) */
	UFUNCTION()
	void OnEnemyDied();

	/** 玩家死亡回调:StopSpawning + 全场敌人 bCanFollow=false + 调用 NewWbp(蓝图重启 UI) */
	UFUNCTION()
	void OnPlayerDied();

	/** 蓝图实现事件:玩家死亡后的重启 UI(由 BP_EnemySpawner 实现具体界面跳转) */
	UFUNCTION(BlueprintImplementableEvent)
	void NewWbp();

	/** 蓝图可调:暂停(关闭玩家输入 + 全场敌人 bCanFollow=false + StopSpawning) */
	UFUNCTION(BlueprintCallable)
	void GameStop();

	/** 蓝图可调:恢复(开启玩家输入 + 全场存活的敌人 bCanFollow=true + StartSpawning) */
	UFUNCTION(BlueprintCallable)
	void GameStart();
};
