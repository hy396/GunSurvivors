
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "Components/CapsuleComponent.h"//碰撞组件
#include "Components/SceneComponent.h"//场景组件
#include "PaperFlipbookComponent.h"//翻页书组件
#include "PaperSpriteComponent.h"//精灵组件

#include "EnhancedInputComponent.h"//增强输入组件
#include "EnhancedInputSubsystems.h"
#include "Components/InputComponent.h"
#include "InputActionValue.h"//输入动作值
#include "GameFramework/Controller.h"//控制器

#include "Engine/TimerHandle.h"
#include "Sound/SoundBase.h"

#include "Bullet.h"

#include "TopdownCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerDiedDelegate);

/**
 * ATopdownCharacter - 俯视角射击游戏玩家
 *
 * 继承自 APawn(不是 ACharacter,因为不需要 CharacterMovementComponent 复杂移动,
 * 自己用 Tick + SetActorLocation 实现纯 2D 平面移动)。
 *
 * 游戏类型:2D 俯视角弹幕射击(Top-down Shooter),鼠标瞄准 + WASD 移动 + 自动射击。
 * 平面约定:游戏世界用 XY-Z 表示(UE 默认 XY 是水平面,Y 是"上",这里 Z 当作屏幕纵轴,
 * Y = 0)。所有运动和瞄准都在 XZ 平面上进行,这是约定俗称的小型 2D 俯视角做法。
 *
 * 子系统组成:
 *   1. 输入系统    Enhanced Input 三件套(MoveAction / ShootAction / WaiGuaAction)
 *   2. 视觉系统    CharacterFlipbook(Idle/Run 切换)+ GunSprite(随鼠标转向)
 *   3. 武器系统    BulletSpawnPosition + BulletActorToSpawn,点击发射 ABullet
 *   4. 移动系统    bCanMove 门控 + MovementDirection 缓存 + 边界裁剪
 *   5. 作弊系统    WaiGuaAction 触发 KaiGua:10 秒高速射击(0.1s/发)+ 1.5 倍移速,
 *                  结束后 15 秒冷却(bIsKaiLe / bIsKaiBa 双 bool 状态机)
 *
 * 状态门控(5 个 bool):
 *   - bCanMove    是否允许移动(死亡时 = false)
 *   - bCanShoot   是否允许射击(冷却中 / 死亡时 = false)
 *   - bIsAlive    是否存活(OverlapBegin 撞敌人 = false,死亡后 OnShootCooldownTimerTimeout 不再复活 bCanShoot)
 *   - bIsKaiLe    是否处于"开挂中"(射击冷却缩短到 GuaShootSpeed,移速 = 150)
 *   - bIsKaiBa    是否处于"开挂冷却"(true 表示 WaiGuaAction 准备好)
 *
 * 死亡流程:玩家与 AEnemy 重叠 → bCanMove/bCanShoot 立刻关闭 + bIsAlive=false +
 *   播放 DieSound + 广播 PlayerDiedDelegate(GameMode 监听,触发游戏结束 / 重启)。
 *
 * @see ABullet                发射的子弹 Actor
 * @see AEnemy                 撞到后死亡的敌人
 * @see AGunSurvivorsGameMode  监听 PlayerDiedDelegate,处理游戏结束
 */
UCLASS()
class GUNSURVIVORS_API ATopdownCharacter : public APawn
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UCapsuleComponent* CapsuleComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UPaperFlipbookComponent* CharacterFlipbook;

	//枪场景组件
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USceneComponent* GunParent;

	//枪精灵组件
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UPaperSpriteComponent* GunSprite;

	//子弹生成位置
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USceneComponent* BulletSpawnPosition;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInputMappingContext* InputMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInputAction* MoveAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInputAction* ShootAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInputAction* WaiGuaAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPaperFlipbook* IdleFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPaperFlipbook* RunFlipbook;

	//开枪音效
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USoundBase* BulletShootSound;

	//死亡音效
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USoundBase* DieSound;
	
	//水平限制
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D HorizontalLimits;

	//垂直限制
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D VerticalLimits;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ABullet> BulletActorToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MovementSpeed = 100.0f;

	//移动方向
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D MovementDirection;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bCanMove = true;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bCanShoot = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsAlive = true;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsKaiLe = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsKaiBa = true;

	//开枪cd
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ShootCooldownDurationInSeconds = 0.85f;

	//开挂射速
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float GuaShootSpeed = 0.1f;

	//开挂时长
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float GuaShootTimeLen = 10.0f;

	//开挂时长
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float GuaShootCDTimeLen = 15.0f;
	
	//开枪定时器
	FTimerHandle ShootCooldownTimer;

	//外挂cd
	FTimerHandle GuaCDTimer;

	//外挂时效
	FTimerHandle GuaTimer;
	
	//角色死亡委托
	FPlayerDiedDelegate PlayerDiedDelegate;
	
	/** 构造函数:创建 CapsuleComp(根) + CharacterFlipbook + GunParent/GunSprite/BulletSpawnPosition */
	ATopdownCharacter();

	/** 初始化:显示鼠标光标 + 注册 Enhanced Input MappingContext + 绑定 Capsule OverlapBegin */
	virtual void BeginPlay() override;

	/** 每帧:把 GunParent 转到鼠标方向(2D 平面 look-at) */
	virtual void Tick(float DeltaTime) override;

	/** 绑定增强输入动作到回调函数(Move Triggered/Completed/Canceled + Shoot Started/Triggered + KaiGua Started) */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** 移动输入触发:更新 MovementDirection + 切换 RunFlipbook + 翻转精灵朝向 + 实际移动(裁剪到边界) */
	void MoveTriggered(const FInputActionValue& Value);

	/** 移动输入结束:MovementDirection 清零 + 切换回 IdleFlipbook(若仍存活) */
	void MoveCompleted(const FInputActionValue& Value);

	/** 射击:bCanShoot 时 spawn ABullet + 计算鼠标方向 Launch + 启动冷却 Timer + 播放音效 */
	void Shoot(const FInputActionValue& Value);

	/** "开挂"动作:仅在 bIsKaiBa 时触发,启动 GuaTimer/GuaCDTimer,期间移速 150、射速 0.1s */
	void KaiGua();

	/** 水平边界裁剪:XPos 是否落在 (HorizontalLimits.X, HorizontalLimits.Y) 之间 */
	bool IsInMapBoundsHorizontal(float XPos);

	/** 垂直边界裁剪:ZPos 是否落在 (VerticalLimits.X, VerticalLimits.Y) 之间 */
	bool IsInMapBoundsVertical(float ZPos);

	/** 射击冷却到期回调:bIsAlive 时重置 bCanShoot = true(死后不再复活) */
	void OnShootCooldownTimerTimeout();

	UFUNCTION()
	void OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
