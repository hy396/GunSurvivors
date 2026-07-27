// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Components/CapsuleComponent.h"
#include "PaperFlipbookComponent.h"

#include "Sound/SoundBase.h"
#include "Engine/TimerHandle.h"

#include "TopdownCharacter.h"

#include "Enemy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEnemyDiedDelegate);

/**
 * AEnemy - 追击型敌人 Actor
 *
 * 由 AEnemySpawner 周期性 spawn,沿直线追踪玩家。
 * 行为很简单:距离 >= StopDistance 时按 MovementSpeed 向玩家移动,
 * 同时根据玩家左右位置翻转 EnemyFlipbook 朝向。
 *
 * 死亡流程(被子弹命中时 ABullet::OverlapBegin → Die):
 *   1. Die() bIsAlive 守卫(防重复调用)
 *   2. bIsAlive=false / bCanFollow=false → Tick 中立刻停止追击
 *   3. EnemyFlipbook 切换到 DeadFlipbookAsset(死亡动画)
 *   4. SetTranslucentSortPriority(-5) 让尸体绘制在地面之下
 *   5. 广播 EnemyDiedDelegate(供 GameMode 计分等监听)
 *   6. PlaySound2D(DieSound)
 *   7. 启动 10s DestroyTimer 兜底销毁
 *
 * 双 bool 状态机:
 *   - bIsAlive     子弹判定用(ABullet::OverlapBegin 检查 Enemy->bIsAlive)
 *   - bCanFollow   追击开关(死亡后置 false,Tick 中立即停止移动)
 *
 * 注意事项:
 *   - 敌人不主动攻击玩家;真正"撞死玩家"的是 ATopdownCharacter::OverlapBegin,
 *     敌人自己只负责移动 + 被子弹打。
 *   - Player 指针默认 nullptr,通常由 AEnemySpawner 在 spawn 后立即赋值(查 GetPlayerController)。
 *
 * @see AEnemySpawner           生成与 Player 赋值
 * @see ABullet::OverlapBegin   命中回调 Die()
 * @see ATopdownCharacter       被追击对象 + 撞到时自己处理死亡
 */
UCLASS()
class GUNSURVIVORS_API AEnemy : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UCapsuleComponent* CapsuleComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UPaperFlipbookComponent* EnemyFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPaperFlipbook* DeadFlipbookAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* DieSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	ATopdownCharacter* Player;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsAlive = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bCanFollow = false;

	//移动速度
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MovementSpeed = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StopDistance = 18.0f;

	//死亡后 10s 兜底销毁 Timer(Die 启动)
	FTimerHandle DestroyTimer;

	//敌人死亡委托(Die 时广播,供外部计分 / 杀敌计数监听)
	FEnemyDiedDelegate EnemyDiedDelegate;

	/** 构造函数:创建 CapsuleComp(根) + EnemyFlipbook(默认无追击) */
	AEnemy();

	/** 初始化(目前为空) */
	virtual void BeginPlay() override;

	/** 每帧:若 alive + 可追击 + Player 有效,按 MovementSpeed 向玩家移动(距离 < StopDistance 时停下)+ 翻转精灵朝向 */
	virtual void Tick(float DeltaTime) override;

	/** 死亡:bIsAlive 守卫 + 切换 DeadFlipbookAsset + 广播委托 + 播放音效 + 启动 10s DestroyTimer */
	void Die();

	/** DestroyTimer 到期回调:调用 Destroy */
	void OnDestroyTimerTimeout();
};
