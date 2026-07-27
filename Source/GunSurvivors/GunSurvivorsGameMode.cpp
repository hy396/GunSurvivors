// Fill out your copyright notice in the Description page of Project Settings.


#include "GunSurvivorsGameMode.h"

#include "Kismet/GameplayStatics.h"

AGunSurvivorsGameMode::AGunSurvivorsGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	SetScore(0);
}

void AGunSurvivorsGameMode::BeginPlay()
{
	Super::BeginPlay();
	SetScore(0);
}

void AGunSurvivorsGameMode::SetScore(int NewScore)
{
	if(NewScore >= 0)
	{
		this->Score = NewScore;
		ScoreChangedDelegate.Broadcast(Score);
	}
}

void AGunSurvivorsGameMode::AddScore(int AmountToAdd)
{
	//this->Score += AmountToAdd;
	int NewScore = Score + AmountToAdd;
	SetScore(NewScore);
	//GEngine->AddOnScreenDebugMessage(-1,5.0f,FColor::Blue,FString::Printf(TEXT("Score: %d"),Score));
}

void AGunSurvivorsGameMode::RestartGame()
{
	GetWorldTimerManager().SetTimer(RestartGameTimer,this, &AGunSurvivorsGameMode::OnRestartGameTimerTimeout,
		1.0f, false, TimeBeforeRestart);
}

void AGunSurvivorsGameMode::OnRestartGameTimerTimeout()
{
	UGameplayStatics::OpenLevel(GetWorld(),FName("MainLevel"));
}
