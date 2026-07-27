
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
	
	ATopdownCharacter();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	void MoveTriggered(const FInputActionValue& Value);
	
	void MoveCompleted(const FInputActionValue& Value);

	void Shoot(const FInputActionValue& Value);

	void KaiGua();

	bool IsInMapBoundsHorizontal(float XPos);
	bool IsInMapBoundsVertical(float ZPos);

	void OnShootCooldownTimerTimeout();

	UFUNCTION()
	void OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
