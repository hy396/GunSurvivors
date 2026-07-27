// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Components/SphereComponent.h"
#include "PaperSpriteComponent.h"
#include "Engine/TimerHandle.h"

#include "Bullet.generated.h"

/**
 * ABullet - 玩家子弹 Actor
 *
 * ATopdownCharacter::Shoot 中 spawn 的子弹,沿 MovementDirection 直线飞行,
 * 命中存活的 AEnemy 时调用 Enemy->Die()。
 *
 * 生命周期:
 *   Launch → bIsLaunched = true + 启动 10s DeleteTimer → 撞敌人或 10s 到期 → 销毁
 *
 * Disable 模式(撞敌人时为什么不直接 Destroy):
 *   - 撞到 AEnemy 时调用 DisableBullet:关闭 SphereComp 碰撞 + 销毁 BulletSprite 组件,
 *     但 actor 本身不立即 Destroy(等 10s DeleteTimer 自然清理)。
 *   - 这是个常见的 OnComponentBeginOverlap 防重入手法:多次重叠同一帧(快速穿透)
 *     不会再触发 Enemy->Die() 多次,因为 bIsDisabled 立即置 true + 碰撞关闭。
 *
 * 注意事项:
 *   - Launch 是"一次性":bIsLaunched 守卫,二次调用直接 return。
 *   - 默认 MovementSpeed = 300,但 ATopdownCharacter::Shoot 实际传入的是 ABullet::MovementSpeed
 *     (蓝图实例值,可能不一样)。
 *
 * @see ATopdownCharacter::Shoot  生成与 Launch
 * @see AEnemy::Die               命中回调
 */
UCLASS()
class GUNSURVIVORS_API ABullet : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UPaperSpriteComponent* BulletSprite;

	UPROPERTY(BlueprintReadWrite)
	FVector2D MovementDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MovementSpeed = 300.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsLaunched = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsDisabled = false;

	//10s 兜底销毁 Timer(Launch 时启动,防止没击中敌人时永久残留)
	FTimerHandle DeleteTimer;

	/** 构造函数:创建 SphereComp(根) + BulletSprite,默认 MovementDirection = (1, 0) */
	ABullet();

	/** 初始化:绑定 SphereComp 的 OnComponentBeginOverlap 事件 */
	virtual void BeginPlay() override;

	/** 每帧:bIsLaunched 时按 MovementDirection * MovementSpeed * DeltaTime 平移(2D XZ 平面) */
	virtual void Tick(float DeltaTime) override;

	/** 发射:首次调用时设置方向/速度 + 启动 10s DeleteTimer(后续调用直接 return) */
	void Launch(FVector2D Direction, float Speed);

	/** DeleteTimer 到期回调:调用 Destroy 清理 actor */
	void OnDeleteTimerTimeout();

	/** 命中碰撞:撞到存活的 AEnemy 时 DisableBullet + 调用 Enemy->Die() */
	UFUNCTION()
	void OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** 失活:关闭碰撞 + 销毁精灵(不立即 Destroy,等 10s DeleteTimer 兜底),bIsDisabled 防重入 */
	void DisableBullet();
};
