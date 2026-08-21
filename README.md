# EBON CROWN

> Unreal Engine 5와 C++로 개발한 3인칭 소울라이크 액션 RPG

플레이어 전투와 성장, 인벤토리, NPC 상호작용, 세이브 포인트, 일반 몬스터 및 보스 AI를 구현한 개인 프로젝트입니다.

단순히 기능을 나열하는 데 그치지 않고 전투, 애니메이션, AI, UI, 아이템 데이터, 저장 시스템이 하나의 게임 플레이 흐름으로 연결되도록 구성했습니다.

---

## 플레이 영상

[![EBON CROWN 플레이 영상](https://img.youtube.com/vi/MsGKZK74F1Q/0.jpg)](https://www.youtube.com/watch?v=MsGKZK74F1Q)

- YouTube: https://www.youtube.com/watch?v=MsGKZK74F1Q

---

## 프로젝트 개요

| 항목 | 내용 |
| --- | --- |
| 프로젝트명 | EBON CROWN |
| 장르 | 3인칭 소울라이크 액션 RPG |
| 개발 형태 | 개인 프로젝트 |
| 개발 기간 | 2025.02 ~ 2026.02 |
| 담당 역할 | 클라이언트 및 게임 플레이 시스템 전반 |
| 엔진 | Unreal Engine 5.5 |
| 개발 언어 | C++, Blueprint |
| 플랫폼 | Windows |

---

## 기술 스택

### Engine & Language

- Unreal Engine 5.5
- C++
- Blueprint

### Gameplay

- Actor Component
- Data Asset
- Animation Montage
- Animation Notify
- Enhanced Input
- SaveGame

### AI

- AI Controller
- Behavior Tree
- Blackboard
- Navigation System
- Gameplay Tasks

### UI & Presentation

- UMG
- Slate
- Level Sequence
- Movie Scene

---

## 핵심 구현 내용

- 플레이어 전투, 스태미나, 능력치, UI, 상호작용, 세이브 포인트 기능을 Actor Component 단위로 분리했습니다.
- 기본 공격, 연속 콤보, 스킬 공격, 회피, 무기 장착과 강화 수치 반영을 구현했습니다.
- Animation Notify를 실제 공격 판정의 기준 시점으로 사용해 애니메이션과 타격 타이밍을 연결했습니다.
- Behavior Tree와 Blackboard를 활용해 탐지, 접근, 공격, 후퇴, 휴식 및 보스 패턴을 구현했습니다.
- Data Asset을 사용해 아이템, 무기, 포션, 스킬 및 콤보 데이터를 게임 로직과 분리했습니다.
- 인벤토리, 무기 장착, 무기 강화, 재화 및 플레이어 성장 시스템을 구현했습니다.
- 반복적으로 사용되는 스킬 이펙트에 Object Pool을 적용했습니다.
- 플레이어 능력치, 재화, 장비, 강화 수치, 세이브 포인트 및 진행 상태를 SaveGame에 저장했습니다.
- NPC 대화, 대장장이, 기도, 능력치 초기화, 숨겨진 NPC 등 상호작용 콘텐츠를 구현했습니다.
- 보스 전투, 컷신, 대화, 레벨 전환 및 월드 이동 흐름을 구성했습니다.

---

# 주요 시스템

## 1. 플레이어 컴포넌트 구조

플레이어에게 필요한 기능을 하나의 Character 클래스에 집중시키지 않고 역할별 Actor Component로 분리했습니다.

| 클래스 | 역할 |
| --- | --- |
| `UPlayerCombatComponent` | 기본 공격, 콤보, 스킬, 무기 장착 및 전투 수치 관리 |
| `UPlayerStaminaComponent` | 스태미나 소비와 회복 |
| `UPlayerStatComponent` | 능력치, 피해, 재화, 레벨업 및 파생 능력치 계산 |
| `UPlayerInteractionComponent` | NPC, 아이템, 세이브 포인트 등 상호작용 대상 탐색 |
| `UPlayerSavePointComponent` | 세이브 포인트 등록, 휴식 및 월드 이동 |
| `UPlayerUIComponent` | 플레이어 상태와 전투 UI 갱신 |
| `UInventoryComponent` | 아이템 보관, 사용 및 장비 연동 |

각 컴포넌트가 자신의 상태와 기능을 담당하도록 구성해 플레이어 클래스의 책임을 줄이고 시스템별 수정과 확장이 쉽도록 설계했습니다.

---

## 2. 플레이어 전투 시스템

`UPlayerCombatComponent`를 중심으로 공격, 콤보, 스킬 및 무기 장착 기능을 관리합니다.

### 주요 기능

- 기본 공격
- 연속 콤보 공격
- 콤보 입력 예약
- 스킬 공격
- 무기 장착 및 해제
- 무기 능력치 적용
- 무기 강화 수치 반영
- 무기 HitBox 활성화 및 비활성화
- 스킬 이펙트 재생
- 공격 종료 후 상태 초기화

### 주요 함수

```cpp
void BasicAttack();
void ComboStart();
void ComboCheck();
void ComboEnd(UAnimMontage* Montage, bool bInterrupted);

void BaseAttackCheck();

void SkillStart();
void SkillAttackCheck();
void SpawnSkillEffect();

void EnableWeaponHitBox();
void DisableWeaponHitBox();

void EquipWeapon(UInventoryItem* WeaponItem);
void UnEquipWeapon(UInventoryItem* WeaponItem);
```

### 공격 처리 흐름

```text
공격 입력
    ↓
현재 플레이어 상태 및 장비 검사
    ↓
공격 Animation Montage 재생
    ↓
Animation Notify 발생
    ↓
무기 HitBox 또는 공격 판정 활성화
    ↓
대상에게 피해 적용
    ↓
UI 및 전투 상태 갱신
    ↓
Montage 종료 후 콤보 상태 초기화
```

공격 버튼을 누른 즉시 피해를 적용하지 않고 실제 애니메이션의 타격 프레임에 배치한 Animation Notify를 기준으로 공격 판정을 실행합니다.

이를 통해 플레이어에게 보이는 무기 움직임과 실제 게임 로직의 타격 시점이 일치하도록 구성했습니다.

---

## 3. 콤보 시스템

연속 공격은 현재 콤보 단계와 추가 입력 여부를 기준으로 진행됩니다.

1. 첫 공격 입력 시 콤보를 시작합니다.
2. 현재 콤보 단계에 맞는 Animation Montage 구간을 재생합니다.
3. 다음 공격을 입력할 수 있는 시간 동안 추가 입력을 확인합니다.
4. 추가 입력이 있으면 다음 콤보 구간으로 이동합니다.
5. 입력이 없거나 마지막 공격이 종료되면 콤보 상태를 초기화합니다.

콤보 입력 가능 시간을 Timer로 관리해 각 공격 애니메이션에 맞는 입력 타이밍을 설정할 수 있도록 구성했습니다.

---

## 4. Animation Notify 기반 공격 판정

공격 판정과 캐릭터 상태 변경을 Animation Notify로 제어했습니다.

### 주요 Notify 클래스

| 클래스 | 역할 |
| --- | --- |
| `AnimNotify_BaseAttackCheck` | 기본 공격 판정 실행 |
| `AnimNotify_SkillAttackCheck` | 스킬 공격 판정 실행 |
| `EnableWeaponHitBox` | 무기 충돌 판정 활성화 |
| `DisableWeaponHitBox` | 무기 충돌 판정 비활성화 |
| `StartRolling` | 회피 시작 상태 적용 |
| `EndRolling` | 회피 종료 상태 적용 |
| `DragonBreath_NotifyState` | 드래곤 브레스 공격 구간 관리 |

공격 Animation과 게임 로직이 서로 다른 시점에 실행되는 문제를 줄이고 애니메이션 에셋에서 실제 판정 시점을 직접 조정할 수 있도록 했습니다.

---

## 5. 스태미나 및 능력치 시스템

`UPlayerStaminaComponent`와 `UPlayerStatComponent`를 분리해 행동 자원과 플레이어 성장 데이터를 관리합니다.

### 능력치 시스템

- 최대 체력
- 현재 체력
- 스태미나
- 공격 관련 능력치
- 무기 보정 수치
- 재화
- 레벨업
- 파생 능력치 재계산
- 저장 데이터 생성 및 복원

### 주요 함수

```cpp
void CalculateDerivedStats();
void InitializeCombatStats();
void RecalculateStatsAfterLevelUp(bool bRefillHPFPStamina = false);

void TakeDamage(int32 DamageAmount);

void AddCurrency(int32 Amount);
bool SpendCurrency(int32 Amount);

void FillSaveData(FPlayerStatSaveData& OutSaveData) const;
void ApplySaveData(const FPlayerStatSaveData& InSaveData);
```

레벨업이나 장비 변경 이후 관련 파생 능력치를 다시 계산하도록 구성했습니다.

---

## 6. 아이템 및 무기 데이터 구조

아이템의 정적 정보를 Data Asset으로 관리합니다.

### 주요 데이터 클래스

| 클래스 | 역할 |
| --- | --- |
| `UTPRItemData` | 모든 아이템 데이터의 기반 클래스 |
| `UWeaponItemData` | 무기 능력치와 장착 데이터 |
| `UPotionItemData` | 포션 효과와 사용 데이터 |
| `USkillData` | 스킬 효과, 위치, 공격 정보 |
| `UMMComboActionData` | 콤보 공격 데이터 |

### Data Asset을 사용한 이유

- 아이템 수치와 게임 로직을 분리할 수 있습니다.
- 새로운 무기와 아이템을 에디터에서 추가할 수 있습니다.
- 아이템마다 다른 데이터를 하나의 구조로 관리할 수 있습니다.
- 밸런스 수정 시 C++ 코드를 직접 수정하는 범위를 줄일 수 있습니다.

---

## 7. 인벤토리 및 장비 시스템

인벤토리 시스템은 아이템 데이터와 실제 인벤토리 항목을 분리해 관리합니다.

### 주요 클래스

- `UInventoryComponent`
- `UInventoryItem`
- `UWeaponListItemObject`
- `UInventoryWidget`
- `USlot`
- `UCurrentEquipedWidget`

### 지원 기능

- 아이템 획득
- 아이템 보관
- 포션 사용
- 무기 장착 및 해제
- 현재 장착 무기 표시
- 아이템 수량 및 상태 표시
- 무기 강화 UI 연동

---

## 8. 무기 강화 시스템

무기별 강화 수치를 별도로 관리하며 강화 결과를 플레이어 전투 능력치에 반영합니다.

```cpp
TMap<FPrimaryAssetId, int32> ItemEnhanceLevels;
```

아이템의 `FPrimaryAssetId`를 Key로 사용해 각 무기의 강화 단계를 저장합니다.

이를 통해 동일한 무기 데이터 구조를 사용하면서도 무기별 강화 상태를 SaveGame에 유지할 수 있도록 구성했습니다.

---

## 9. Object Pool 기반 스킬 이펙트

전투 중 반복적으로 사용되는 스킬 이펙트를 매번 생성하고 제거하면 객체 생성 및 해제 비용이 누적될 수 있습니다.

이를 줄이기 위해 Object Pool 구조를 적용했습니다.

### 주요 클래스

- `ASkillEffectPool`
- `APooledSkillEffectActor`

### 처리 방식

1. 게임 시작 시 사용할 이펙트 Actor를 미리 생성합니다.
2. 이펙트가 필요하면 비활성 상태의 객체를 가져옵니다.
3. 위치와 상태를 초기화한 뒤 활성화합니다.
4. 재생이 끝나면 객체를 제거하지 않고 비활성화합니다.
5. 반환된 객체는 다음 스킬에서 다시 사용합니다.

### 기본 풀 크기

```cpp
int32 PoolSize = 20;
```

`UPlayerCombatComponent` 내부의 파티클 이펙트 풀은 기본 크기 10으로 구성되어 있습니다.

```cpp
int32 SkillEffectPoolSize = 10;
float SkillEffectLifeTime = 2.0f;
```

Object Pool을 통해 반복적인 `SpawnActor()`와 `Destroy()` 호출을 줄이고 객체의 생성 시점과 재사용 과정을 직접 관리했습니다.

---

## 10. 일반 몬스터 AI

일반 몬스터는 AI Controller, Behavior Tree, Blackboard, Service 및 Task를 사용해 동작합니다.

### 주요 AI 클래스

| 클래스 | 역할 |
| --- | --- |
| `AEnemyAIController` | Behavior Tree 실행 및 AI 제어 |
| `AEnemyCharacter` | 일반 몬스터의 전투 상태와 능력치 |
| `BTService_DetectPlayer` | 플레이어 탐지 |
| `BTService_CheckAttackChance` | 공격 가능 여부 및 공격 확률 검사 |
| `BTService_UpdateHP` | 현재 체력 상태 갱신 |
| `BTTask_PlayAttackMontage` | 공격 Animation Montage 재생 |
| `BTTask_CircleAroundPlayer` | 플레이어 주변 이동 |
| `BTTask_Retreat` | 공격 후 후퇴 |
| `BTTask_Rest` | 공격 사이 대기 |
| `BTTask_SetBlackboardBool` | Blackboard 상태 변경 |

### AI 전투 흐름

```text
플레이어 탐지
    ↓
Blackboard에 대상 저장
    ↓
플레이어에게 접근
    ↓
공격 가능 거리 검사
    ↓
공격 또는 주변 이동 선택
    ↓
공격 Montage 재생
    ↓
후퇴 또는 휴식
    ↓
다음 행동 결정
```

단순히 플레이어에게 직선으로 접근해 공격하는 구조에서 벗어나 주변 이동, 후퇴, 휴식 Task를 추가해 전투 간격과 위치를 조절했습니다.

---

## 11. 보스 AI 및 패턴

일반 몬스터 구조를 기반으로 보스별 전용 클래스를 확장했습니다.

### 주요 보스 클래스

- `ABossCharacter`
- `ABossDragon`
- `ALastBoss`

### 주요 기능

- 보스 전용 공격 패턴
- 체력 상태에 따른 행동 변화
- 공격 패턴 선택
- 보스 전용 Animation Montage
- 보스 전투 시작 Trigger
- 보스 전투 시퀀스
- 보스 사망 및 진행 상태 저장
- 숨겨진 보스 해금 상태 관리

### 보스 관련 클래스

| 클래스 | 역할 |
| --- | --- |
| `ABossBattleInstance` | 보스 전투 상태 관리 |
| `ABossSequenceTrigger` | 보스 연출과 시퀀스 시작 |
| `BTTask_PlayBossPattern` | Behavior Tree에서 보스 패턴 실행 |
| `ABossDragon` | 드래곤 보스 전용 공격 |
| `ALastBoss` | 최종 보스 전용 로직 |

---

## 12. NPC 및 상호작용 시스템

플레이어가 바라보거나 접근한 상호작용 대상을 탐색하고 대상에 맞는 기능을 실행합니다.

### NPC 클래스

| 클래스 | 역할 |
| --- | --- |
| `ANPC` | NPC 공통 기능 |
| `ABlackSmith` | 무기 강화 |
| `APrayerNPC` | 기도 및 관련 상호작용 |
| `AResetStatNPC` | 플레이어 능력치 초기화 |
| `AHiddenNPC` | 숨겨진 이벤트 및 진행 조건 |

### 상호작용 대상

- NPC
- 아이템
- 세이브 포인트
- 엘리베이터
- 레벨 전환 Trigger
- 튜토리얼 Trigger
- 대화 및 선택지

---

## 13. 세이브 포인트 및 월드 이동

세이브 포인트는 단순한 저장 기능 외에도 회복, 성장 및 월드 이동의 중심 역할을 담당합니다.

### 주요 기능

- 세이브 포인트 활성화
- 플레이어 상태 저장
- 체력 및 스태미나 회복
- 레벨업 메뉴
- 발견한 세이브 포인트 목록 관리
- 활성화된 지점 간 월드 이동
- 사망 후 체크포인트 복귀

### 관련 클래스

- `ASavePoint`
- `UPlayerSavePointComponent`
- `USavePointMenu`
- `ULevelUpMenu`
- `UWorldTravelMenu`
- `UWorldTravelMenuEntry`

---

## 14. 저장 시스템

`UTPRSaveGame`을 사용해 게임 진행 상태를 저장합니다.

### 저장 데이터

- 플레이어 능력치
- 현재 재화
- 능력치 성장에 사용한 재화
- 인벤토리 아이템
- 장착 무기
- 무기별 강화 수치
- 활성화한 세이브 포인트
- 발견한 월드 이동 지점
- 숨겨진 보스 해금 상태
- 게임 진행 및 이벤트 상태

### 주요 저장 변수

```cpp
TArray<FString> ActivatedSavePointNames;
TArray<FSavePointInfo> AllDiscoveredSavePoints;
TMap<FPrimaryAssetId, int32> ItemEnhanceLevels;

bool bHiddenBossUnlocked = false;
```

게임을 재실행한 이후에도 플레이어의 성장, 장비 및 월드 진행 상태가 유지되도록 구성했습니다.

---

## 15. UI 시스템

플레이어 상태와 게임 시스템을 UMG 기반 Widget으로 표시합니다.

### 주요 UI

| 클래스 | 역할 |
| --- | --- |
| `UHPBar` | 체력 표시 |
| `UPlayerStatusWidget` | 플레이어 능력치 표시 |
| `UCurrencyWidget` | 보유 재화 표시 |
| `UInventoryWidget` | 인벤토리 |
| `UCurrentEquipedWidget` | 현재 장착 무기 |
| `UInteractionWidget` | 상호작용 안내 |
| `UDialogueWidget` | NPC 대화 |
| `UDialogueChoiceWidget` | 대화 선택지 |
| `UDeathScreenWidget` | 사망 화면 |
| `UWeaponUpgradeWidget` | 무기 강화 |
| `UPauseMenuWidget` | 일시정지 메뉴 |
| `UTutorialWidget` | 튜토리얼 안내 |

플레이어 능력치나 재화가 변경될 때 관련 UI가 갱신되도록 게임 로직과 Widget을 연결했습니다.

---

## 16. 레벨 및 연출 시스템

Level Sequence와 Trigger Actor를 사용해 보스 전투, 스토리 및 레벨 전환 흐름을 구성했습니다.

### 관련 클래스

| 클래스 | 역할 |
| --- | --- |
| `ALevelConvertTrigger` | 다른 레벨로 전환 |
| `ALevelPreloadTrigger` | 필요한 레벨 사전 로드 |
| `AStoryCutsceneManager` | 스토리 컷신 관리 |
| `AStoryDialogueManager` | 스토리 대화 관리 |
| `ATutorialWidgetTrigger` | 튜토리얼 UI 표시 |
| `ABossSequenceTrigger` | 보스 전투 연출 시작 |

---

# 트러블 슈팅

## 반복적인 이펙트 생성 및 제거 비용

### 문제

전투 중 반복적으로 사용되는 스킬 이펙트를 매번 `SpawnActor()`로 생성하고 재생이 끝난 뒤 `Destroy()`하는 방식은 객체 생성 및 해제 비용이 계속 발생하는 문제가 있었습니다.

### 해결

게임 시작 시 일정 수의 이펙트 Actor를 미리 생성해 Pool에 저장하고 필요할 때 비활성 객체를 꺼내 재사용하도록 변경했습니다.

이펙트 재생이 끝나면 객체를 제거하지 않고 다시 비활성화해 Pool로 반환하도록 구성했습니다.

### 결과

반복적인 객체 생성과 제거 호출을 줄이고 이펙트의 생성 시점, 초기화 및 재사용 과정을 시스템에서 직접 관리할 수 있게 되었습니다.

### 배운 점

반복적으로 생성되는 객체는 기능 구현뿐만 아니라 생성 빈도와 생명주기까지 고려해야 안정적인 게임 시스템을 구성할 수 있다는 점을 배웠습니다.

---

## 공격 애니메이션과 실제 타격 시점 불일치

### 문제

공격 입력 직후 판정을 실행하면 화면에 보이는 무기의 타격 시점과 실제 피해 적용 시점이 일치하지 않는 문제가 발생했습니다.

### 해결

공격 정보를 먼저 저장하고 공격 Animation Montage를 재생한 뒤 `AnimNotify_BaseAttackCheck`와 `AnimNotify_SkillAttackCheck`가 발생하는 시점에 실제 판정을 실행하도록 변경했습니다.

무기 충돌이 필요한 구간에는 `EnableWeaponHitBox`와 `DisableWeaponHitBox` Notify를 배치했습니다.

### 결과

공격 Animation과 실제 타격 판정 시점을 일치시키고 공격 구간이 아닌 프레임에서 불필요한 충돌이 발생하는 문제를 줄였습니다.

### 배운 점

전투 시스템에서는 입력 시점뿐만 아니라 애니메이션, 판정, 이펙트와 사운드가 실행되는 공통 이벤트 시점을 명확하게 정하는 것이 중요하다는 점을 배웠습니다.

---

## 반복적인 UI 직접 참조

### 문제

플레이어 상태가 변경될 때마다 여러 게임 클래스가 Widget을 직접 탐색하고 수정하면 클래스 간 의존성이 증가하고 UI 갱신 코드가 여러 위치에 분산되는 문제가 있었습니다.

### 해결

플레이어의 상태 변경과 UI 갱신 책임을 분리하고 `UPlayerUIComponent` 및 상태 변경 이벤트를 통해 관련 Widget이 필요한 데이터를 갱신하도록 구성했습니다.

### 결과

게임 로직이 특정 Widget 구현을 직접 참조하는 범위를 줄이고 UI 변경 시 수정해야 하는 코드의 범위를 줄였습니다.

### 배운 점

게임 상태를 변경하는 객체와 상태를 화면에 표현하는 객체를 분리해야 시스템 확장과 유지보수가 쉬워진다는 점을 배웠습니다.

---

# 프로젝트 구조

```text
Source/Third_Person_RPG/
├─ Actor/
│  ├─ NPC/
│  │  ├─ BlackSmith.*
│  │  ├─ HiddenNPC.*
│  │  ├─ PrayerNPC.*
│  │  └─ ResetStatNPC.*
│  ├─ Object/
│  │  ├─ ElevatorPlatform.*
│  │  └─ ElevatorTrigger.*
│  ├─ Pool/
│  │  ├─ PooledSkillEffectActor.*
│  │  └─ SkillEffectPool.*
│  ├─ BossBattleInstance.*
│  ├─ BossSequenceTrigger.*
│  ├─ LevelConvertTrigger.*
│  ├─ LevelPreloadTrigger.*
│  ├─ NPC.*
│  ├─ SavePoint.*
│  ├─ StoryCutsceneManager.*
│  └─ StoryDialogueManager.*
│
├─ Animation/
│  ├─ AnimNotify/
│  │  ├─ AnimNotify_BaseAttackCheck.*
│  │  ├─ AnimNotify_SkillAttackCheck.*
│  │  ├─ EnableWeaponHitBox.*
│  │  ├─ DisableWeaponHitBox.*
│  │  ├─ StartRolling.*
│  │  └─ EndRolling.*
│  ├─ MMBaseAnimInstance.*
│  └─ MMPlayerAnimInstance.*
│
├─ Character/
│  ├─ BT/
│  │  ├─ BTService_CheckAttackChance.*
│  │  ├─ BTService_DetectPlayer.*
│  │  ├─ BTService_UpdateHP.*
│  │  ├─ BTTask_CircleAroundPlayer.*
│  │  ├─ BTTask_PlayAttackMontage.*
│  │  ├─ BTTask_PlayBossPattern.*
│  │  ├─ BTTask_Rest.*
│  │  └─ BTTask_Retreat.*
│  ├─ BossCharacter.*
│  ├─ BossDragon.*
│  ├─ EnemyAIController.*
│  ├─ EnemyCharacter.*
│  ├─ LastBoss.*
│  ├─ PlayerCharacter.*
│  └─ TPRPlayerController.*
│
├─ Component/
│  ├─ PlayerCombatComponent.*
│  ├─ PlayerInteractionComponent.*
│  ├─ PlayerSavePointComponent.*
│  ├─ PlayerStaminaComponent.*
│  ├─ PlayerStatComponent.*
│  └─ PlayerUIComponent.*
│
├─ Data/
│  ├─ ItemData/
│  │  ├─ PotionItemData.*
│  │  ├─ TPRItemData.*
│  │  └─ WeaponItemData.*
│  ├─ MMComboActionData.*
│  └─ SkillData.*
│
├─ Inventory/
│  ├─ InventoryComponent.*
│  ├─ InventoryItem.*
│  └─ WeaponListItemObject.*
│
├─ Item/
│  ├─ Weapon/
│  │  └─ TPRWeapon.*
│  └─ Item.*
│
├─ UI/
│  ├─ InventoryUI/
│  ├─ SavePointUI/
│  ├─ Start/
│  ├─ Tutorial/
│  ├─ CurrencyWidget.*
│  ├─ DeathScreenWidget.*
│  ├─ DialogueWidget.*
│  ├─ HPBar.*
│  ├─ PlayerStatusWidget.*
│  └─ WeaponUpgradeWidget.*
│
├─ GameMode/
├─ Instance/
├─ Interface/
├─ TPRSaveGame.*
└─ Third_Person_RPG.Build.cs
```

---

# 실행 방법

## 요구 환경

- Unreal Engine 5.5
- Visual Studio 2022
- Desktop development with C++
- Windows 10/11
- Git LFS

## 프로젝트 실행

```bash
git lfs install
git clone https://github.com/choijiuk1017/EBON_CROWN.git
cd EBON_CROWN
```

1. `Third_Person_RPG.uproject`를 우클릭합니다.
2. `Generate Visual Studio project files`를 실행합니다.
3. 생성된 Solution을 Visual Studio 2022로 엽니다.
4. `Development Editor / Win64` 구성으로 빌드합니다.
5. `Third_Person_RPG.uproject`를 실행합니다.

> 저장소에 대용량 에셋이 포함되어 있어 최초 Clone, 프로젝트 로딩 및 Shader Compile에 시간이 걸릴 수 있습니다.

---

# 개발 과정에서 얻은 경험

- Actor Component를 활용한 플레이어 기능 분리
- Animation Notify 기반 공격 판정 시점 제어
- Data Asset 기반 아이템과 스킬 데이터 관리
- Behavior Tree와 Blackboard 기반 전투 AI 구현
- 일반 몬스터와 보스 AI의 공통 구조 및 확장 방식 설계
- Object Pool을 활용한 반복 이펙트 객체 재사용
- SaveGame을 이용한 플레이어 성장 및 월드 진행 상태 저장
- UMG 기반 인벤토리, 성장, 대화 및 세이브 포인트 UI 구현
- Level Sequence와 Trigger를 이용한 게임 진행 및 연출 구성

