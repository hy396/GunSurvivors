

#include "TopdownCharacter.h"

#include "Enemy.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

ATopdownCharacter::ATopdownCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	SetRootComponent(CapsuleComp);

	CharacterFlipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("CharacterFlipbook"));
	CharacterFlipbook->SetupAttachment(RootComponent);

	GunParent = CreateDefaultSubobject<USceneComponent>(TEXT("GunParent"));
	GunParent->SetupAttachment(RootComponent);

	GunSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("GunSprite"));
	GunSprite->SetupAttachment(GunParent);

	BulletSpawnPosition = CreateDefaultSubobject<USceneComponent>(TEXT("BulletSpawnPosition"));
	BulletSpawnPosition->SetupAttachment(GunSprite);
}

void ATopdownCharacter::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if(PlayerController)
	{
		//始终显示鼠标
		PlayerController->SetShowMouseCursor(true);
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem)
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}

	CapsuleComp->OnComponentBeginOverlap.AddDynamic(this,&ATopdownCharacter::OverlapBegin);
}

void ATopdownCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// if(bCanMove)
	// {
	// 	// if(MovementDirection.Length() > 0.0f)
	// 	// {
	// 	// 	if(MovementDirection.Length() > 1.0f)
	// 	// 	{
	// 	// 		//同时按下两个按键
	// 	// 		MovementDirection.Normalize();//归一化
	// 	// 	}
	// 	// 	//移动的距离等于MoveActionValue乘以速度
	// 	// 	FVector2D DistanceToMove = MovementDirection * MovementSpeed * DeltaTime;
	// 	//
	// 	// 	FVector CurrentLocation = GetActorLocation();
	// 	// 	FVector NewLocation = CurrentLocation + FVector(DistanceToMove.X, 0.0f, 0.0f);
	// 	// 	if (!IsInMapBoundsHorizontal(NewLocation.X))
	// 	// 	{
	// 	// 		NewLocation -= FVector(DistanceToMove.X, 0.0f, 0.0f);
	// 	// 	}
	// 	//
	// 	// 	NewLocation += FVector(0.0f, 0.0f, DistanceToMove.Y);
	// 	// 	if (!IsInMapBoundsVertical(NewLocation.Z))
	// 	// 	{
	// 	// 		NewLocation -= FVector(0.0f, 0.0f, DistanceToMove.Y);
	// 	// 	}
	// 	//
	// 	// 	SetActorLocation(NewLocation);
	// 	// }
	// }

	//转枪
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if(PlayerController)
	{
		FVector MouseWorldLocation, MouseWorldDirection;//鼠标位置，世界坐标位置
		PlayerController->DeprojectMousePositionToWorld(MouseWorldLocation, MouseWorldDirection);

		FVector CurrentLocation = GetActorLocation();
		FVector Start = FVector(CurrentLocation.X, 0.0f, CurrentLocation.Z);
		FVector Target = FVector(MouseWorldLocation.X, 0.0f, MouseWorldLocation.Z);
		FRotator GunParentRotator = UKismetMathLibrary::FindLookAtRotation(Start, Target);

		GunParent->SetRelativeRotation(GunParentRotator);
	}
}

void ATopdownCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent)
	{
		//Triggered 动作已触发。这意味着它完成了所有触发器要求的求值
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATopdownCharacter::MoveTriggered);
		//Completed触发器求值过程已完成
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ATopdownCharacter::MoveCompleted);
		//Canceled触发已取消。例如，在"按住"动作还没触发之前，用户就松开了按钮
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Canceled, this, &ATopdownCharacter::MoveCompleted);
		//Started发生了开始触发器求值的某个事件。例如，"双击"触发器的第一次按键将调用一次"已开始"状态。
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, this, &ATopdownCharacter::Shoot);
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Triggered, this, &ATopdownCharacter::Shoot);

		EnhancedInputComponent->BindAction(WaiGuaAction, ETriggerEvent::Started, this, &ATopdownCharacter::KaiGua);
	}
}

void ATopdownCharacter::MoveTriggered(const FInputActionValue& Value)
{
	FVector2D MoveActionValue = Value.Get<FVector2D>();
	if(bCanMove)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::Printf(TEXT("移动速度：%f"),MovementSpeed));
		MovementDirection = MoveActionValue;
		//设置状态
		CharacterFlipbook->SetFlipbook(RunFlipbook);

		FRotator FlipbookRotation = CharacterFlipbook->GetComponentRotation();
		if(MovementDirection.X < 0.0f)
		{
			//(Pitch=0.000000,Yaw=180.000000,Roll=0.000000)
			if(FlipbookRotation.Yaw == 0)
			{
				CharacterFlipbook->SetWorldRotation(FRotator(0.0f,180.0f,0.0f));
			}
		}else if(MovementDirection.X > 0.0f)
		{
			if(FlipbookRotation.Yaw == 180)
			{
				CharacterFlipbook->SetWorldRotation(FRotator(0.0f,0.0f,0.0f));
			}
		}
		if(MovementDirection.Length() > 0.0f)
		{
			if(MovementDirection.Length() > 1.0f)
			{
				//同时按下两个按键
				MovementDirection.Normalize();//归一化
			}
			//移动的距离等于MoveActionValue乘以速度
			FVector2D DistanceToMove = MovementDirection * MovementSpeed * GetWorld()->DeltaTimeSeconds;

			FVector CurrentLocation = GetActorLocation();
			FVector NewLocation = CurrentLocation + FVector(DistanceToMove.X, 0.0f, 0.0f);
			if (!IsInMapBoundsHorizontal(NewLocation.X))
			{
				NewLocation -= FVector(DistanceToMove.X, 0.0f, 0.0f);
			}

			NewLocation += FVector(0.0f, 0.0f, DistanceToMove.Y);
			if (!IsInMapBoundsVertical(NewLocation.Z))
			{
				NewLocation -= FVector(0.0f, 0.0f, DistanceToMove.Y);
			}

			SetActorLocation(NewLocation);
		}
	}
}

void ATopdownCharacter::MoveCompleted(const FInputActionValue& Value)
{
	//停止移动
	MovementDirection = FVector2D(0.0f, 0.0f);
	if(bIsAlive)
	{
		//设置状态
		CharacterFlipbook->SetFlipbook(IdleFlipbook);
	}
}

void ATopdownCharacter::Shoot(const FInputActionValue& Value)
{
	if(bCanShoot)
	{
		//GEngine->AddOnScreenDebugMessage(-1,5.0f,FColor::White,TEXT("涩涩"));
		bCanShoot = false;
		// 子弹生成
		ABullet *Bullet = GetWorld()->SpawnActor<ABullet>(BulletActorToSpawn,
			BulletSpawnPosition->GetComponentLocation(),
			FRotator(0.0f, 0.0f, 0.0f));
		check(Bullet);//检测子弹是否有效，适用于开发，不适合用于生产

		// 获取鼠标的世界坐标
		APlayerController* PlayerController = Cast<APlayerController>(Controller);
		check(PlayerController);
		FVector MouseWorldLocation, MouseWorldDirection;
		PlayerController->DeprojectMousePositionToWorld(MouseWorldLocation, MouseWorldDirection);

		//计算子弹方向
		FVector CurrentLocation = GetActorLocation();
		FVector2D BulletDirection = FVector2D(MouseWorldLocation.X - CurrentLocation.X, MouseWorldLocation.Z - CurrentLocation.Z);
		BulletDirection.Normalize();

		// 发射子弹
		//float BulletSpeed = 300.0f;
		Bullet->Launch(BulletDirection, Bullet->MovementSpeed);

		if(bIsKaiLe)
		{
			//内置cd设置，使用定时器
			GetWorldTimerManager().SetTimer(ShootCooldownTimer, this, &ATopdownCharacter::OnShootCooldownTimerTimeout, 1.0f, false, GuaShootSpeed);
		}else
		{
			//内置cd设置，使用定时器
			GetWorldTimerManager().SetTimer(ShootCooldownTimer, this, &ATopdownCharacter::OnShootCooldownTimerTimeout, 1.0f, false, ShootCooldownDurationInSeconds);
		}
		UGameplayStatics::PlaySound2D(GetWorld(),BulletShootSound);
	}
}

void ATopdownCharacter::KaiGua()
{
	if(bIsKaiBa)
	{
		bIsKaiLe = true;
		bIsKaiBa = false;
		MovementSpeed = 150.0f;
		GetWorldTimerManager().SetTimer(GuaCDTimer, [this]()
		{
			bIsKaiBa = true;
		}, 1.0f, false, GuaShootCDTimeLen);
		GetWorldTimerManager().SetTimer(GuaTimer, [this]()
		{
			MovementSpeed = 100.0f;
			bIsKaiLe = false;
		}, 1.0f, false, GuaShootTimeLen);
	}
}

bool ATopdownCharacter::IsInMapBoundsHorizontal(float XPos)
{
	bool Result = true;

	Result = (XPos > HorizontalLimits.X) && (XPos < HorizontalLimits.Y);

	return Result;
}

bool ATopdownCharacter::IsInMapBoundsVertical(float ZPos)
{
	bool Result = true;

	Result = (ZPos > VerticalLimits.X) && (ZPos < VerticalLimits.Y);

	return Result;
}

void ATopdownCharacter::OnShootCooldownTimerTimeout()
{
	if(bIsAlive)
	{
		bCanShoot = true;
	}
}

void ATopdownCharacter::OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AEnemy *Enemy =	Cast<AEnemy>(OtherActor);
	if(Enemy && Enemy->bIsAlive)
	{
		if(bIsAlive)
		{
			bCanMove = false;
			bCanShoot = false;
			bIsAlive = false;
			UGameplayStatics::PlaySound2D(GetWorld(),DieSound);
			PlayerDiedDelegate.Broadcast();
		}
	}
}

