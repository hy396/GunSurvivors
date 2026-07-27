// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "Engine/TimerHandle.h"

#include "GunSurvivorsGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FScoreChangedDelegate, int, NewScore);
/**
 * 
 */
UCLASS()
class GUNSURVIVORS_API AGunSurvivorsGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int Score = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeBeforeRestart = 1.0f;

	//在蓝图中使用需要BlueprintAssignable
	UPROPERTY(BlueprintAssignable)
	FScoreChangedDelegate ScoreChangedDelegate;

	FTimerHandle RestartGameTimer;
	
	AGunSurvivorsGameMode();
	virtual void BeginPlay() override;

	void SetScore(int NewScore);
	void AddScore(int AmountToAdd);

	void RestartGame();
	void OnRestartGameTimerTimeout();
};
