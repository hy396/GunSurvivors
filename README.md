# 🔫 GunSurvivors（枪火幸存者）

> 一款基于 **Unreal Engine 5.3** + **Paper2D** 制作的 2D 俯视角射击 Roguelite 小游戏(类《Vampire Survivors》)。操控角色在封闭场地内不断向四面八方涌来的敌人开火,击杀越多、敌潮越密,直到倒下为止。

![UE](https://img.shields.io/badge/Unreal_Engine-5.3-0D47A1?logo=unrealengine&logoColor=white)
![Platform](https://img.shields.io/badge/Platform-Windows-0078D6?logo=windows)
![Language](https://img.shields.io/badge/C++-17-00599C?logo=c%2B%2B&logoColor=white)
![Plugin](https://img.shields.io/badge/Plugin-ModelingTools-00A86B)
![Status](https://img.shields.io/badge/Status-学习项目-orange)

---

## 📑 目录

- [🎮 游戏概览](#-游戏概览)
- [✨ 游戏特色](#-游戏特色)
- [🛠 技术栈](#-技术栈)
- [💻 系统要求](#-系统要求)
- [🚀 快速开始](#-快速开始)
- [🎯 玩法与操作](#-玩法与操作)
- [📁 项目结构](#-项目结构)
- [🏗 核心架构](#-核心架构)
- [🧩 关键设计模式](#-关键设计模式)
- [🛠 开发指南](#-开发指南)
- [🐞 已知问题与 TODO](#-已知问题与-todo)
- [📜 许可证](#-许可证)
- [🙏 致谢](#-致谢)

---

## 🎮 游戏概览

**GunSurvivors** 是一个完整的 2D 俯视角 Roguelite 射击 Demo,演示了 UE5 中 2D 俯视角射击、动态难度缩放、模块化对象池的轻量实现方式。

- 🗺 **2 个关卡**:`Xinshou`(新手教学) → `MainLevel`(主战场)
- 👾 **无限敌潮**:玩家周围圆形区域随机刷新敌人
- 📈 **动态难度**:每生成 `DifficultySpawnInterval` 个敌人,刷怪间隔/移速/玩家射速全部提升
- 💥 **「开挂」 Buff**:内置一次性冷却的高射速爆发技能
- 💀 **死亡结算 UI**:阵亡后弹出结算界面,可一键重开
- 🖱 **自定义鼠标光标**:通过 UMG 蓝图替换系统光标

---

## ✨ 游戏特色

### 🎯 鼠标瞄准射击
- 枪精灵(gun)实时朝鼠标方向旋转
- 子弹沿 `FVector2D` 方向直线飞行,基于 Sphere 碰撞
- 子弹击中敌人后自我禁用并销毁,避免重复触发

### 🤖 AI 追击 + 距离停止
- 敌人每帧计算与玩家距离
- 距离 > `StopDistance` 时朝玩家移动
- 进入停止距离后停步,让玩家可以贴身作战

### 📈 动态难度曲线
- 每击杀 10 个敌人触发一次难度上升
- 同时调整 3 个数值:
  - `SpawnTime`(刷怪间隔)↓ 至最小值
  - `EnemySpeed`(敌人移速)↑ 至最大值
  - `ShootCooldownDurationInSeconds`(玩家射速)↓ 至最小值

### 💥 Buff 系统
- 「开挂」技能:临时提升移动速度 + 大幅加快射速
- 持续时间 `GuaShootTimeLen` = 10s
- 冷却时间 `GuaShootCDTimeLen` = 15s
- 用 `bIsKaiLe` / `bIsKaiBa` 双标志位管理状态

### 🖼 UMG 状态机 UI
- **WBP_HUD**:战斗中常驻的 HP/得分/计时
- **WBP_daoshu**:倒数字幕
- **WBP_Xinshou / WBP_Xinshou1 / WBP_Xinshou_end**:教学关流程
- **WBP_jiesuan / WBP_Over / WBP_Over2**:死亡结算
- **WBP_tui**:退场 UI

### 🎮 增强输入
- 三个 Input Action:`IA_Move` / `IA_Shoot` / `IA_KaiGua`
- `IMC_GunSurvivors` 集中管理键位

---

## 🛠 技术栈

| 类别 | 技术 |
| --- | --- |
| 引擎 | Unreal Engine 5.3 |
| 渲染 | Paper2D(`UPaperFlipbookComponent`、`UPaperSpriteComponent`) |
| 输入 | Enhanced Input 系统 |
| 物理 | UE Chaos + `UCapsuleComponent` / `USphereComponent` |
| UI | UMG(`UUserWidget`、自定义鼠标光标) |
| 音频 | `USoundBase` + 2D 播放 |
| 模块依赖 | `Core`、`CoreUObject`、`Engine`、`InputCore`、`EnhancedInput`、`Paper2D` |
| C++ 标准 | UE C++(`PCHUsage = UseExplicitOrSharedPCHs`) |

---

## 💻 系统要求

### 运行环境
- **操作系统**:Windows 10 / 11 (64-bit)
- **GPU**:支持 DirectX 12(默认 DX12,SM6;DX11 可降级)
- **硬件目标**:Desktop / Maximum(详见 `DefaultEngine.ini`)
- **内存**:≥ 8 GB RAM
- **硬盘**:≥ 5 GB 可用空间

### 开发环境
- **Unreal Engine 5.3**(Epic Games Launcher 安装)
- **Visual Studio 2022**(组件见 `.vsconfig`):
  - `Microsoft.Net.Component.4.6.2.TargetingPack`
  - `Microsoft.VisualStudio.Component.VC.Tools.x86.x64`
  - `Microsoft.VisualStudio.Component.Windows10SDK.22000`
  - `Microsoft.VisualStudio.Workload.NativeGame`
- **Git**(可选,用于克隆仓库)

---

## 🚀 快速开始

### 方式一:在 UE 编辑器中打开(推荐)

```bash
# 1. 克隆仓库
git clone https://github.com/<your-username>/GunSurvivors.git
cd GunSurvivors

# 2. 双击 GunSurvivors.uproject,UE 编辑器会自动启动并加载项目
#    (首次打开会触发 C++ 模块编译)

# 3. 编辑器加载完成后,按工具栏 ▶ Play 即可运行游戏
```

### 方式二:命令行构建

```powershell
# 1. 生成 Visual Studio 项目文件
"<UE_INSTALL_DIR>/Engine/Binaries/DotNET/UnrealBuildTool/UnrealBuildTool.exe" ^
    -projectfiles ^
    -project="%CD%/GunSurvivors.uproject" ^
    -game -engine -progress

# 2. 编译编辑器模块
"<UE_INSTALL_DIR>/Engine/Build/BatchFiles/Build.bat" ^
    GunSurvivorsEditor Win64 Development ^
    -project="%CD%/GunSurvivors.uproject" -waitmutex

# 3. 启动编辑器
"<UE_INSTALL_DIR>/Engine/Binaries/Win64/UnrealEditor.exe" ^
    "%CD%/GunSurvivors.uproject"

# 4. (可选)打包为独立可执行文件
"<UE_INSTALL_DIR>/Engine/Build/BatchFiles/RunUAT.bat" ^
    BuildCookRun -project="%CD%/GunSurvivors.uproject" ^
    -platform=Win64 -configuration=Shipping -build -cook -stage -package -archive
```

> 💡 将 `<UE_INSTALL_DIR>` 替换为你的 UE 5.3 安装路径,例如 `C:/Program Files/Epic Games/UE_5.3`。

### ⚠️ 首次打开常见问题

| 现象 | 解决方案 |
| --- | --- |
| 找不到 `BP_Player` 等蓝图 | 确认 `Content/BP/` 完整,不要删除 `Content/` 目录 |
| 鼠标不显示或显示默认光标 | 检查 `DefaultEngine.ini` 中 `SoftwareCursors` 指向 `BP_widget_shubiao` |
| C++ 编译失败 | 通过 Visual Studio Installer 安装 `.vsconfig` 列出的所有组件 |
| 教学关卡无法触发 | 确认关卡中放置了 `BP_GameMode` 且 `Default Pawn Class` 设为 `BP_Player` |

---

## 🎯 玩法与操作

### 键位

| 操作 | 键位 | 说明 |
| --- | --- | --- |
| 🏃 移动 | `W` / `A` / `S` / `D` 或方向键 | 八方向移动,自动切换 Idle/Run 动画 |
| 🎯 瞄准 + 射击 | 鼠标移动 + 鼠标左键 | 枪口实时朝鼠标方向,点击发射子弹 |
| 💥 开挂 Buff | `Q` / `Space` | 进入高速射击 + 加速状态,持续 10s,冷却 15s |

### 玩法循环

```
进入关卡
   ↓
玩家移动 + 鼠标点击射击
   ↓
敌人从四周圆形区域不断刷新
   ↓
击杀敌人 +10 分
   ↓
每击杀 10 个 → 难度提升(刷更快、追更紧、射更急)
   ↓
被敌人贴身 → 死亡 → 结算 UI
   ↓
重开 → MainLevel
```

### 敌人行为

| 行为 | 触发条件 |
| --- | --- |
| 追击玩家 | `bIsAlive && bCanFollow && Player != nullptr` 且距离 > `StopDistance` |
| 停止移动 | 距离 ≤ `StopDistance` |
| 死亡播放动画 | 被子弹击中(`ABullet::OverlapBegin` → `Enemy->Die()`) |
| 销毁 | 死亡后 10 秒(让死亡动画播完) |

### 「开挂」Buff 数值

| 状态 | 移动速度 | 射击冷却 |
| --- | --- | --- |
| 平时 | 100.0 | 0.85s |
| 开挂中 | 150.0 | 0.1s |

---

## 📁 项目结构

```
GunSurvivors/
├── GunSurvivors.uproject         # UE 项目描述文件
├── GunSurvivors.sln              # Visual Studio 解决方案
├── .vsconfig                     # VS 工作组件清单
├── Config/                       # 引擎配置
│   ├── DefaultEngine.ini         # 默认关卡、GameMode、渲染设置
│   ├── DefaultGame.ini           # 打包、压缩、Cook 列表
│   ├── DefaultInput.ini          # 增强输入轴配置
│   ├── DefaultEditor.ini
│   └── Layouts/
├── Source/
│   └── GunSurvivors/             # C++ 源码(单模块)
│       ├── GunSurvivors.Build.cs         # 模块依赖
│       ├── GunSurvivors.{h,cpp}          # 模块入口
│       ├── GunSurvivorsGameMode.{h,cpp}  # GameMode + 分数 + 重开
│       ├── TopdownCharacter.{h,cpp}      # 玩家 Pawn
│       ├── Bullet.{h,cpp}                # 子弹 Actor
│       ├── Enemy.{h,cpp}                 # 敌人 Actor
│       ├── EnemySpawner.{h,cpp}          # 敌人生成器 + 难度调节
│       ├── GunSurvivors.Target.cs
│       └── GunSurvivorsEditor.Target.cs
├── Content/                      # UE 资源
│   ├── Assets/                   # 源美术/音频
│   │   ├── Flipbooks/            # PlayerIdle/Run、EnemyRun/Dead
│   │   ├── Sprites/              # Arena/Enemy/Gun/Player 源贴图
│   │   ├── Sounds/               # aBullet、aDeath
│   │   ├── Textures/
│   │   └── icon/
│   ├── BP/                       # 蓝图资源
│   │   ├── BP_Player.uasset              # 玩家 Pawn
│   │   ├── BP_Enemy.uasset / BP_Enemy1   # 敌人实例
│   │   ├── BP_Bullet.uasset              # 子弹实例
│   │   ├── BP_EnemySpawner.uasset        # 生成器
│   │   ├── BP_GunSurvivorsGameMode.uasset
│   │   ├── BP_Arena.uasset               # 战斗场地
│   │   ├── BP_widget_shubiao.uasset      # 自定义鼠标光标
│   │   ├── WBP_HUD.uasset                # 主 HUD
│   │   ├── WBP_daoshu.uasset             # 倒数字幕
│   │   ├── WBP_jiesuan.uasset / WBP_Over / WBP_Over2 / WBP_tui  # 死亡/结算 UI
│   │   └── WBP_Xinshou / WBP_Xinshou1 / WBP_Xinshou_end  # 教学 UI
│   ├── Input/                    # IA_Move / IA_Shoot / IA_KaiGua + IMC
│   ├── Levels/                   # Xinshou.umap / MainLevel.umap
│   ├── Collections/
│   └── Developers/
└── Build/                        # UBT 中间产物
```

---

## 🏗 核心架构

### 类关系图

```
APawn
 └── ATopdownCharacter         # 玩家(继承 APawn,非 ACharacter)

AActor
 ├── ABullet                   # 子弹
 ├── AEnemy                    # 敌人
 └── AEnemySpawner             # 生成器

AGameModeBase
 └── AGunSurvivorsGameMode     # 持有分数 + 重开逻辑
```

### 数据流:玩家射击

```
鼠标点击
  ↓
ATopdownCharacter::Shoot()
  ├─ 校验 bCanShoot
  ├─ SpawnActor<ABullet>(BP_Bullet, BulletSpawnPosition, ...)
  ├─ 计算 MouseWorldLocation → BulletDirection(FVector2D)
  ├─ Bullet->Launch(BulletDirection, MovementSpeed)
  └─ SetTimer(ShootCooldownTimer, GuaShootSpeed / ShootCooldownDurationInSeconds)
  ↓
ABullet::Tick()
  └─ SetActorLocation(Current + Direction * Speed * Δt)
  ↓
ABullet::OverlapBegin(命中 AEnemy)
  ├─ DisableBullet()(关闭碰撞 + 销毁精灵)
  └─ AEnemy::Die()
        ├─ bIsAlive = false
        ├─ SetFlipbook(DeadFlipbookAsset)
        ├─ PlaySound2D(DieSound)
        └─ SetTimer(DestroyTimer, 10s 后销毁)
  ↓
AEnemy::EnemyDiedDelegate.Broadcast()
  ↓
AEnemySpawner::OnEnemyDied()
  └─ MyGameMode->AddScore(10) → 触发 GameMode 分数委托
```

### 数据流:难度升级

```
AEnemySpawner::SpawnEnemy()
  ├─ 在玩家周围 SpawnDistance 半径内随机一点
  ├─ SpawnActor<AEnemy>(BP_Enemy, ...)
  ├─ SetupEnemy()(设置 Player 引用 + MovementSpeed)
  ├─ TotalEnemyCount++
  └─ if (TotalEnemyCount % DifficultySpawnInterval == 0):
        ├─ SpawnTime -= DecreaseSpawnTimerByEveryInterval(最低 SpawnTimeMinimumLimit)
        ├─ EnemySpeed += NanduUpSpeed(最高 MaxEnemySpeed)
        ├─ Player->ShootCooldownDurationInSeconds -= ShootSpeed(最低 MinShootSpeed)
        └─ StopSpawning() + StartSpawning()(刷新定时器)
```

### 数据流:玩家死亡

```
ATopdownCharacter::OverlapBegin(碰到 AEnemy)
  ├─ bCanMove = false; bCanShoot = false; bIsAlive = false
  ├─ PlaySound2D(DieSound)
  └─ PlayerDiedDelegate.Broadcast()
        ↓
AEnemySpawner::OnPlayerDied()
  ├─ StopSpawning()
  ├─ 遍历所有 AEnemy → bCanFollow = false
  └─ NewWbp()(蓝图事件,弹出结算 UI)
```

---

## 🧩 关键设计模式

### 1. 枪口朝向 — LookAtRotation

每帧 `Tick` 中把鼠标世界坐标反投影到 2D 平面,作为目标点:

```cpp
FVector MouseWorldLocation, MouseWorldDirection;
PlayerController->DeprojectMousePositionToWorld(MouseWorldLocation, MouseWorldDirection);
FRotator GunParentRotator = UKismetMathLibrary::FindLookAtRotation(Start, Target);
GunParent->SetRelativeRotation(GunParentRotator);
```

### 2. 子弹自我禁用 + 计时销毁

击中时**不立即销毁 Actor**,而是关闭碰撞并销毁精灵,延迟 10s 再 `Destroy()`。这样可避免击中瞬间的多次 `OverlapBegin` 触发,也让死亡动画能完整播放:

```cpp
void ABullet::DisableBullet() {
    SphereComp->SetCollisionEnabled(NoCollision);
    BulletSprite->DestroyComponent();
}
```

### 3. 标志位门控

`ATopdownCharacter` 的所有输入处理函数都先检查 `bIsAlive` / `bCanMove` / `bCanShoot`:

```cpp
void ATopdownCharacter::Shoot(const FInputActionValue& Value) {
    if (bCanShoot) { ... }
}
```

### 4. 双标志位管理 Buff

`bIsKaiLe`(激活中)与 `bIsKaiBa`(冷却完毕)互锁,避免在 Buff 持续期间重复触发:

```cpp
void ATopdownCharacter::KaiGua() {
    if (bIsKaiBa) {       // 冷却完毕才能开
        bIsKaiLe = true;  // 进入 Buff
        bIsKaiBa = false; // 进入冷却
        // 15s 后 bIsKaiBa = true
        // 10s 后 bIsKaiLe = false(Buff 结束)
    }
}
```

### 5. 动态委托广播分数变化

```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FScoreChangedDelegate, int, NewScore);
UPROPERTY(BlueprintAssignable)
FScoreChangedDelegate ScoreChangedDelegate;
```

蓝图 HUD 绑定该委托,`SetScore` 触发即刷新 UI,无需 Tick 轮询。

### 6. SetupEnemy() 工厂模式

`AEnemySpawner::SetupEnemy(AEnemy*)` 在生成后统一注入依赖(`Player` 引用、`MovementSpeed`、委托绑定),避免每个 `AEnemy` 在 `BeginPlay` 中重复 `GetActorOfClass` 查询:

```cpp
void AEnemySpawner::SetupEnemy(AEnemy* Enemy) {
    Enemy->Player = Player;
    Enemy->bCanFollow = true;
    Enemy->MovementSpeed = EnemySpeed;
    Enemy->EnemyDiedDelegate.AddDynamic(this, &AEnemySpawner::OnEnemyDied);
}
```

### 7. 自定义鼠标光标

`DefaultEngine.ini` 中通过 UMG 蓝图替换系统光标:

```ini
SoftwareCursors=((Default, "/Game/BP/BP_widget_shubiao.BP_widget_shubiao_C"))
```

---

## 🛠 开发指南

### 如何添加一种新敌人

1. 在 `Source/GunSurvivors/` 复制 `Enemy.h`/`.cpp` 创建 `MyEnemy.h`/`.cpp`,继承 `AActor`。
2. 在 `Content/BP/` 复制 `BP_Enemy` 创建 `BP_MyEnemy`,设置 Flipbook / DieSound。
3. 在 `AEnemySpawner::SetupEnemy` 中根据类型 switch 设置不同参数。

### 如何新增一种 Buff

1. 在 `ATopdownCharacter` 中仿照 `KaiGua()` 增加新方法,使用新的 `FTimerHandle`。
2. 添加对应的 `UInputAction`(`Content/Input/IA_*.uasset`)并加入 `IMC_GunSurvivors`。
3. 在 `SetupPlayerInputComponent` 中 `BindAction`。
4. 在 `BP_Player` 蓝图中将新 IA 资产绑定到 `ATopdownCharacter` 的 `WaiGuaAction` 槽位(或新字段)。

### 如何调节难度曲线

所有难度参数都在 `AEnemySpawner` 上,直接在 BP 中调整:

| 参数 | 默认 | 含义 |
| --- | --- | --- |
| `SpawnTime` | 1.0s | 初始刷怪间隔 |
| `SpawnTimeMinimumLimit` | 0.4s | 刷怪间隔下限 |
| `DecreaseSpawnTimerByEveryInterval` | 0.05s | 每 10 杀缩短的间隔 |
| `EnemySpeed` | 50.0 | 敌人初始移速 |
| `MaxEnemySpeed` | 80.0 | 敌人移速上限 |
| `NanduUpSpeed` | 2.5 | 每 10 杀提速 |
| `ShootSpeed` | 0.05s | 每 10 杀玩家射速提升量 |
| `MinShootSpeed` | 0.25s | 玩家射速上限 |
| `DifficultySpawnInterval` | 10 | 多少杀触发一次难度 |

### 调试小贴士

- 控制台 `~` → `stat fps` / `stat unit` 看性能
- `r.ScreenPercentage 50` 降分辨率排查性能瓶颈
- `GameMode::RestartGame()` 在死亡时调用,延迟 1s 后 `OpenLevel("MainLevel")` 重开
- `NewWbp` 是 `BlueprintImplementableEvent`,在 BP_GunSurvivorsGameMode 中可绑定具体弹哪个 WBP

---

## 🐞 已知问题与 TODO

- [ ] 敌人寻路是直线追击,无障碍物躲避,会被墙卡住
- [ ] 子弹击中后立即销毁精灵组件,但 `Tick` 仍可能在同帧移动一次(无视觉影响,但浪费一次计算)
- [ ] `AEnemySpawner::BeginPlay` 用 `GetActorOfClass` 单例查找玩家,场景中有多名玩家时会拿到第一个
- [ ] `AGunSurvivorsGameMode::RestartGame` 硬编码 `OpenLevel("MainLevel")`,应改为 `UGameplayStatics::OpenLevel(GetWorld(), FName(*GetWorld()->GetName()))` 或在 GameMode 暴露关卡名字段
- [ ] 「开挂」Buff 持续时间与冷却时间固定,建议改为可配置 + 道具拾取触发
- [ ] 缺少 XP/升级系统(类 Vampire Survivors 的核心循环)
- [ ] 缺少存档/最高分记录
- [ ] 死亡结算 UI 切换有 1s 延迟(`TimeBeforeRestart`),期间玩家可以继续操作
- [ ] `AEnemy::Tick` 实时计算 `PlayerLocation`,无对象池,大量敌人时性能下降明显

---

## 📜 许可证

本项目仅供学习与个人作品展示使用。

- **代码部分**(`Source/`)可参考使用,商用请自行评估。
- **美术/音频资源**(`Content/Assets/`)来源于公开的 Kenney.nl / OpenGameArt 风格资源,版权归原作者所有,如有商用需求请联系原作者。
- 本项目不含任何第三方付费插件,所有依赖均为 UE 自带模块或免费开源资源。

---

## 🙏 致谢

- 🎮 [Unreal Engine](https://www.unrealengine.com/) — Epic Games
- 🎨 美术资源风格参考 [Kenney](https://kenney.nl/) / [OpenGameArt](https://opengameart.org/)
- 📚 灵感来源 [Vampire Survivors](https://store.steampowered.com/app/1794680/Vampire_Survivors/) — poncle
- 📖 感谢所有开源 UE 学习社区

---

<p align="center">
  Made with ❤️ and 🔫
</p>
