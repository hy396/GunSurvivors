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

	FTimerHandle DestroyTimer;

	//委托
	FEnemyDiedDelegate EnemyDiedDelegate;
	
	AEnemy();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	
	void Die();
	void OnDestroyTimerTimeout();
};
