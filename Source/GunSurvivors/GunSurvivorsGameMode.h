// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "Engine/TimerHandle.h"

#include "GunSurvivorsGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FScoreChangedDelegate, int, NewScore);

/**
 * AGunSurvivorsGameMode - 游戏主模式(计分 + 重启)
 *
 * 项目唯一的 GameMode,职责:
 *   1. 维护玩家分数 Score
 *   2. 暴露 ScoreChangedDelegate 给 UMG 计分 UI(BlueprintAssignable)
 *   3. 游戏结束后调度重启到 "MainLevel" 关卡
 *
 * 事件流:
 *   AEnemySpawner::OnEnemyDied → GameMode->AddScore(10)
 *   AddScore → SetScore(NewScore)
 *   SetScore 仅接受 NewScore >= 0(负数静默丢弃,不广播)
 *   SetScore → ScoreChangedDelegate.Broadcast(Score) → UMG 刷新显示
 *
 * 重启流程:
 *   AEnemySpawner::OnPlayerDied → NewWbp(蓝图 UI)→ 用户点击 → GameMode->RestartGame
 *   RestartGame → TimeBeforeRestart 秒后 OnRestartGameTimerTimeout → UGameplayStatics::OpenLevel("MainLevel")
 *
 * 注意事项:
 *   - "MainLevel" 是写死的关卡名,如果改了地图名要同步修改这里(或改成读取 DefaultEngine 的 Map.Map)
 *   - SetScore 的"非负"约束:目前没有调用者会传入负数,但是未来若要支持"扣分"需要先去掉这个守卫
 *
 * @see AEnemySpawner::OnEnemyDied  计分来源
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

	//重启关卡的延迟 Timer(RestartGame 启动)
	FTimerHandle RestartGameTimer;

	/** 构造函数:启用 Tick + 初始化 Score = 0 */
	AGunSurvivorsGameMode();

	/** 初始化:再次 SetScore(0)(防御 BeginPlay 之前的状态污染) */
	virtual void BeginPlay() override;

	/** 设置分数:仅当 NewScore >= 0 时赋值并广播 ScoreChangedDelegate(负数静默丢弃) */
	void SetScore(int NewScore);

	/** 加分:内部计算 Score + AmountToAdd 后调用 SetScore */
	void AddScore(int AmountToAdd);

	/** 重启调度:启动 TimeBeforeRestart 秒后回调 OnRestartGameTimerTimeout */
	void RestartGame();

	/** 重启 Timer 到期回调:OpenLevel 到写死的 "MainLevel" 关卡 */
	void OnRestartGameTimerTimeout();
};
