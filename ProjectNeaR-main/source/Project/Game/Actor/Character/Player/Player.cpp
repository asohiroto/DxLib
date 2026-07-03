#include "Player.h"
#include "PlayerStateBase.h"
#include "PlayerStateIdle.h"
#include "PlayerStateStart.h"
#include "PlayerStateAvoid.h"
#include "PlayerStateFall.h"
#include "AvoidColl.h"
#include "Weapon/Weapon.h"
#include "../Enemy/EnemyManager.h"
#include "../Enemy/EnemyBase.h"
#include "../../../../General/game.h"
#include "../../../../General/Collision/CapsuleCollider.h"
#include "../../../../General/Collision/Rigidbody.h"
#include "../../../../General/Collision/Collidable.h"
#include "../../../../General/CharaStatus.h"
#include "../../../../General/Input.h"
#include "../../../../General/Model.h"
#include "../../../../General/Animator.h"
#include "../../../../General/Collision/Physics.h"
#include "../../ActorManager.h"
#include "../../../Attack/AttackManager.h"
#include "../../../Camera/PlayerCamera.h"
#include "../../../../General/CSV/CSVDataLoader.h"
#include "../../../../General/CSV/CSVData.h"
#include "../../../../General/Sound/SoundManager.h"
#include "../../../../General/Effect/EffekseerManager.h"
#include <DxLib.h>
#include <cmath>
#include <cassert>
#include "../../../../General/ShaderPostProcess.h"
#include "../../../../Main/Application.h"
#include "../../../../General/AssetManager.h"
#include "../../../../Game/UI/Player/PlayerHPUI.h"
#include "../../../../Game/UI/Player/HealCommandU.h"
#include "../../../../Game/UI/UIManager.h"

namespace
{
	//プレイヤーのパスデータ数
	constexpr int kPathNum = 5;

	//回避判定倍率
	constexpr float kAvoidCollRadiusRate = 5.0f;

	//ジャンプの最大数
	constexpr int kMaxJumpNum = 2;

	//武器の持ち手
	constexpr int kHandIndex = 295;
	//背中
	constexpr int kBackLightIndex = 302;
	constexpr int kBackBigIndex = 304;

	//武器を収めるまでのフレーム
	constexpr float kPutAwayFrame = 60.0f * 5.0f;

	//ヒットグリッジ
	constexpr float kHitGlitchFrame = 20.0f;
	constexpr float kHitGlitchStrengt = 0.5f;
	constexpr float kHitGlitchScale = 100.0f;
	constexpr float kHitGlitchSpeed = 10.0f;
	//ピンチの時は2倍
	constexpr float kPinchRate = 2.0f;
	//グリッチフレーム
	constexpr float kGlitchPinchFrame = 10.0f;
	constexpr float kGlitchStartFrame = 230.0f;
	constexpr float kGlitchEndFrame = kGlitchStartFrame + kHitGlitchFrame;
	
	//落下した際のリスポーン地点の位置調整
	constexpr float kReturnPushBackDistance = 500.0f;

	//回復可能回数
	constexpr int kFullRecoveryNum = 3;

	//回復Effect
	const std::wstring kHealEffect = L"Heal";
	//回復SE
	const std::wstring kHealSE = L"Heal";
}

Player::Player(std::shared_ptr<ActorData> actorData, std::shared_ptr<CharaStatusData> charaStatusData, std::weak_ptr<ActorManager> pActorManager) :
	CharacterBase(actorData,charaStatusData,Shape::Capsule,pActorManager),
	m_jumpNum(0),
	m_isAvoidable(true),
	m_isJustAvoided(false),
	m_noDamageFrame(0.0f),
	m_putAwayCountFrame(0.0f),
	m_haveWeaponType(AnimData::WeaponType::None),
	m_isAirAttacked(false),
	m_isDraw(true),
	m_glitchFrame(0.0f),
	m_glitchCountFrame(0.0f),
	m_glitchScale(0.0f),
	m_glitchSpeed(0.0f),
	m_glitchkStrengt(0.0f),
	m_totalJustAvoidNum(0),
	m_isHitGlitch(false),
	m_isGoal(false),
	m_respawnPos(Vector3::Zero()),
	m_fullRecoveryNum(kFullRecoveryNum)
{
	//初期リスポーン地点
	SetRespawnPos(GetPos());
	//回避判定作成
	m_avoidColl = std::make_shared<AvoidColl>(m_actorData->GetCollRadius() * kAvoidCollRadiusRate, m_id);
}

Player::~Player()
{
}

void Player::Init()
{
	//初期化
	m_jumpNum = 0;
	m_isAvoidable = true;
	m_isJustAvoided = false;
	m_noDamageFrame = 0.0f;
	m_putAwayCountFrame = 0.0f;
	m_haveWeaponType = AnimData::WeaponType::None;
	m_isAirAttacked = false;
	m_isDraw = true;
	m_glitchFrame = 0.0f;
	m_glitchCountFrame = 0.0f;
	m_glitchScale = 0.0f;
	m_glitchSpeed = 0.0f;
	m_glitchkStrengt = 0.0f;
	m_totalJustAvoidNum = 0;
	m_isHitGlitch = false;
	m_isGoal = false;
	m_isDelete = false;
	m_fullRecoveryNum = kFullRecoveryNum;

	//必要なパスを取得
	auto& csvLoader = CSVDataLoader::GetInstance();
	auto pathData = csvLoader.LoadCSV(m_actorData->GetCSVPathData().c_str()).front()->GetData();

	assert(pathData.size() >= kPathNum);
	//共通初期化
	CharacterBase::Init(pathData[0].c_str(), pathData[1].c_str(),pathData[2].c_str(), pathData[3].c_str(), pathData[4].c_str());

	//待機状態にする(最初はプレイヤー内で状態を初期化するがそのあとは各状態で遷移する
	auto thisPointer = std::dynamic_pointer_cast<Player>(shared_from_this());
	m_state = std::make_shared<PlayerStateIdle>(thisPointer);
	//状態を変化する
	m_state->ChangeState(m_state);
	//モデルの高さ調整
	m_model->SetModelHeightAdjust(-m_actorData->GetCollRadius());

	//体力UI
	auto playerHPUI = std::make_shared<PlayerHPUI>(m_charaStatus);
	playerHPUI->Init();
	//回復コマンドUI
	auto healCommandU = std::make_shared<HealCommandU>(thisPointer);
	healCommandU->Init();

	//体力回復
	m_charaStatus->FullRecovery();

	//リスポーン位置に移動
	Respawn();

	//回避判定登録
	m_avoidColl->Init();
}

void Player::Update()
{
	//入力の取得
	auto& input = Input::GetInstance();

	if (!m_pActorManager.expired() && !GetPlayerCamera().expired() && !m_state->IsWait())
	{
		auto actorM = m_pActorManager.lock();
		auto camera = GetPlayerCamera().lock();
		//ターゲットの検索
		SearchTarget(input, camera,actorM->GetEnemyManager()->GetEnemys());
	}
	
#if _DEBUG
	if (input.IsTrigger("FullRecovery"))
	{
		//復活
		m_charaStatus->FullRecovery();
		//数フレーム無敵
		SetNoDamageFrame(60.0f);
	}
	
	if (input.IsTrigger("Pinch"))
	{
		//ピンチに
		m_charaStatus->SetNowHP(m_charaStatus->GetMaxHP() / 3);
	}
	if (input.IsTrigger("Death"))
	{
		//死亡
		m_charaStatus->SetNowHP(0);
	}
	if (input.IsTrigger("TargetDeath"))
	{
		if (m_targetInfo.m_isFound)
		{
			m_targetInfo.m_pTarget.lock()->GetCharaStatus()->OnDamage(99999, 99999, CharaStatus::AttackWeight::Heaviest);
		}
	}
	if (input.IsTrigger("Strongest"))
	{
		m_charaStatus->SetAT(9999);
		m_charaStatus->SetDF(9999);
		m_charaStatus->SetMS(100);
	}

	////タイムスケール
	//if (input.IsTrigger("Y"))
	//{
	//	m_rb->SetIsMyTimeScale(!m_rb->IsMyTimeScale());
	//	if (m_rb->IsMyTimeScale())
	//	{
	//		SetTimeScale(0.5f);
	//	}
	//	else
	//	{
	//		SetTimeScale(1.0f);
	//	}
	//}

	//メイン攻撃
	//if (input.IsTrigger("X"))
	//{
	//	HaveLightSword();
	//}
	////サブ攻撃
	//if (input.IsTrigger("Y"))
	//{
	//	HaveBigSword();
	//}
#endif

	//回復
	Heal(input);

	//武器を収める
	UpdatePutAwayWeapon();

	//回避中に回避判定に攻撃が当たったなら
	if (m_avoidColl->IsHit())
	{
		m_charaStatus->SetIsHit(true);
	}

	//状態に合わせた更新
	m_state->Update();
	//状態が変わったかをチェック
	if (m_state != m_state->GetNextState())
	{
		//状態を変化する
		m_state = m_state->GetNextState();
		m_state->Init();
	}
	//アニメーションの更新
	m_model->Update();

	//体力に応じてポストエフェクトをかける
	UpdateHit();
	//状態のリセット
	m_charaStatus->InitHitState();

	//ジャスト回避無敵時間
	UpdateJustAvoid();

	//武器を描画するか
	if (!m_pBigSword.expired() && !m_pLightSword.expired())
	{
		m_pBigSword.lock()->SetIsDraw(m_isDraw);
		m_pLightSword.lock()->SetIsDraw(m_isDraw);
	}

	//回避判定
	m_avoidColl->Update(GetNextPos(), std::dynamic_pointer_cast<PlayerStateAvoid>(m_state) != nullptr);
}


void Player::OnCollide(const std::shared_ptr<Collidable> other)
{
}

void Player::Draw() const
{
#if _DEBUG
	
	//リスポーン地点
	DrawSphere3D(
		m_respawnPos.ToDxLibVector(),
		m_actorData->GetCollRadius(),
		16,
		0x00ff00,
		0x00ff00,
		false
	);
	
	//見てる方向
	if (m_model)
	{
		auto forward = m_model->GetDir();
		forward = forward * m_charaStatus->GetSearchRange();
		//視野角
		auto viewDir1 = Quaternion::AngleAxis(m_charaStatus->GetSearchAngle() / 2.0f, Vector3::Up()) * forward;
		auto viewDir2 = Quaternion::AngleAxis(-m_charaStatus->GetSearchAngle() / 2.0f, Vector3::Up()) * forward;
		//描画
		DrawLine3D(m_rb->m_pos.ToDxLibVector(), (m_rb->m_pos + forward).ToDxLibVector(), 0xff0000);
		DrawLine3D(m_rb->m_pos.ToDxLibVector(), (m_rb->m_pos + viewDir1).ToDxLibVector(), 0xff0000);
		DrawLine3D(m_rb->m_pos.ToDxLibVector(), (m_rb->m_pos + viewDir2).ToDxLibVector(), 0xff0000);
	}

	//衝突判定
	DrawCapsule3D(
		m_rb->m_pos.ToDxLibVector(),
		std::dynamic_pointer_cast<CapsuleCollider>(m_collisionData)->GetEndPos().ToDxLibVector(),
		std::dynamic_pointer_cast<CapsuleCollider>(m_collisionData)->GetRadius(),
		16,
		0xff0000,
		0xff0000,
		false
	);

	//回避判定
	m_avoidColl->Draw();
#endif
	

	if (m_isDraw)
	{
		m_model->Draw();
	}
}

void Player::DrawShadow() const
{
	if (m_isDraw)
	{
		m_model->Draw();
	}
}

void Player::Complete()
{
	m_rb->SetPos(m_rb->GetNextPos());
	Vector3 endPos = m_rb->m_pos;
	endPos.y += m_actorData->GetCollHeight();
	std::dynamic_pointer_cast<CapsuleCollider>(m_collisionData)->SetEndPos(endPos);//カプセルの移動
	//モデルの座標更新
	m_model->SetPos(m_rb->m_pos.ToDxLibVector());
	m_model->ApplyMat();
	//ジャンプ回数リセット
	if (m_isFloor)
	{
		m_jumpNum = 0;
	}
	//カメラに情報を渡す
	if (!GetPlayerCamera().expired())
	{
		auto camera = GetPlayerCamera().lock();
		camera->SetPlayerPos(GetCenterPos());
		camera->SetPlayerVec(m_rb->GetMoveVec());
		camera->SetPlayerDir(m_model->GetDir());
	}

#if _DEBUG
	if (m_charaStatus->IsHit())
	{
		printf("プレイヤーの残りの体力 : %d\n", m_charaStatus->GetNowHP());
	}
#endif
}


void Player::End()
{
	//モデル削除
	m_model->End();
	//登録解除
	Collidable::End();
	m_avoidColl->End();
}

void Player::Heal(Input& input)
{
	if (input.GetPOVInfo().IsDownTrigger())
	{
		//体力が満タンでなく、かつ死んでいなければ回復
		if (m_charaStatus->GetNowHP() < m_charaStatus->GetMaxHP() &&
			!m_charaStatus->IsDead() &&
			m_fullRecoveryNum > 0)
		{
			//回復エフェクト
			EffekseerManager::GetInstance().CreateTrackActorEffect(GetEffectPath(kHealEffect), 
				std::dynamic_pointer_cast<Actor>(shared_from_this()));
			//回復SE
			SoundManager::GetInstance().PlayOnceSE(GetSEPath(kHealSE));
			//全回復
			m_charaStatus->FullRecovery();
			//回復可能回数を減らす
			--m_fullRecoveryNum;
		}
	}
}

Quaternion Player::GetCameraRot() const
{
	Quaternion q = Quaternion::IdentityQ();
	if (!m_pActorManager.expired())
	{
		auto actorM = m_pActorManager.lock();
		if (!actorM->GetPlayerCamera().expired())
		{
			q = actorM->GetPlayerCamera().lock()->GetRotH();
		}
	}
	return q;
}

void Player::AddJumpNum()
{
	++m_jumpNum;
	m_jumpNum = MathSub::ClampInt(m_jumpNum, 0, kMaxJumpNum);
}

bool Player::IsJumpable() const
{
	return m_jumpNum < kMaxJumpNum;
}

bool Player::IsFall() const
{
	return m_rb->m_vec.y < 0.0f;
}

void Player::SetNoDamageFrame(float m_frame)
{
	m_noDamageFrame = m_frame;
	m_isJustAvoided = true;
}

void Player::UpdateJustAvoid()
{
	//ジャスト回避後
	if (m_isJustAvoided)
	{
		m_noDamageFrame -= GetTimeScale();
		m_charaStatus->SetIsNoDamage(true);
		//無敵時間が切れて今回避状態ではないなら無敵解除
		if (m_noDamageFrame <= 0.0f && std::dynamic_pointer_cast<PlayerStateAvoid>(m_state) == nullptr)
		{
			m_isJustAvoided = false;
			m_charaStatus->SetIsNoDamage(false);
		}
	}
#if _DEBUG
	/*if (m_charaStatus->IsNoDamage())
	{
		printf("今無敵です\n");
	}
	else
	{
		printf("今無敵ではない\n");
	}*/
#endif
}

void Player::SetSword(std::weak_ptr<Weapon> weapon, bool isLightSword)
{
	if (weapon.expired())return;
	auto sword = weapon.lock();
	sword->SetOwnerHandle(m_model->GetModelHandle());
	if (isLightSword)
	{
		m_pLightSword = weapon;
		sword->SetFrameIndex(kHandIndex, kBackLightIndex);
	}
	else
	{
		m_pBigSword = weapon;
		sword->SetFrameIndex(kHandIndex, kBackBigIndex);
	}
	
}

std::weak_ptr<Weapon> Player::GetWeapon(AnimData::WeaponType type) const
{
	if (type == AnimData::WeaponType::LightSword)
	{
		return m_pLightSword;
	}
	else
	{
		return m_pBigSword;
	}
}

void Player::HaveLightSword()
{
	//片手剣を持つ
	if (m_pLightSword.expired() || m_pBigSword.expired())return;

	auto lightSword = m_pLightSword.lock();
	auto bigSword = m_pBigSword.lock();

	lightSword->SetIsBattle(true);
	bigSword->SetIsBattle(false);

	//武器を収めるまでのフレーム
	m_putAwayCountFrame = kPutAwayFrame;

	//片手剣を持っている
	m_haveWeaponType = AnimData::WeaponType::LightSword;
}

void Player::HaveBigSword()
{
	//大剣を持つ
	if (m_pLightSword.expired() || m_pBigSword.expired())return;

	auto lightSword = m_pLightSword.lock();
	auto bigSword = m_pBigSword.lock();

	lightSword->SetIsBattle(false);
	bigSword->SetIsBattle(true);

	//武器を収めるまでのフレーム
	m_putAwayCountFrame = kPutAwayFrame;

	//大剣を持っている
	m_haveWeaponType = AnimData::WeaponType::BigSword;
}

void Player::PutAwaySword()
{
	//武器を収める
	if (m_pLightSword.expired() || m_pBigSword.expired())return;

	auto lightSword = m_pLightSword.lock();
	auto bigSword = m_pBigSword.lock();

	lightSword->SetIsBattle(false);
	bigSword->SetIsBattle(false);

	//武器を持っていない
	m_haveWeaponType = AnimData::WeaponType::None;
}

std::string Player::GetAnim(std::wstring state)const
{
	std::string path = "Player|";

	//探す
	return CharacterBase::GetAnim(state, path, m_haveWeaponType);
}

bool Player::IsGliding() const
{
	auto state = std::dynamic_pointer_cast<PlayerStateFall>(m_state);
	if (state != nullptr)
	{
		return state->IsGliding();
	}
	return false;
}


float Player::GetRadius() const
{
	return std::dynamic_pointer_cast<CapsuleCollider>(m_collisionData)->GetRadius();
}

void Player::SetRespawnPos(Vector3 pos)
{
	Vector3 respownPos = pos;
	respownPos.y += m_actorData->GetCollRadius();
	m_respawnPos = respownPos;
}

void Player::Respawn()
{
	//リスポーン地点に移動
	m_rb->SetPos(m_respawnPos);
	Vector3 endPos = m_rb->m_pos;
	endPos.y += m_actorData->GetCollHeight();
	std::dynamic_pointer_cast<CapsuleCollider>(m_collisionData)->SetEndPos(endPos);
	m_rb->SetVec(Vector3::Zero());
	//ロックオン解除
	if (!GetPlayerCamera().expired())
	{
		auto camera = GetPlayerCamera().lock();
		ResetTarget(camera);
	}
}

void Player::SetStartState()
{
	//スタート状態にする
	auto thisPointer = std::dynamic_pointer_cast<Player>(shared_from_this());
	m_state = std::make_shared<PlayerStateStart>(thisPointer);
	//状態を変化する
	m_state->ChangeState(m_state);
}

bool Player::IsStartState() const
{
	return std::dynamic_pointer_cast<PlayerStateStart>(m_state) != nullptr;
}

std::weak_ptr<PlayerCamera> Player::GetPlayerCamera() const
{
	std::weak_ptr<PlayerCamera> camera;
	if (!m_pActorManager.expired())
	{
		auto actorM = m_pActorManager.lock();
		if (!actorM->GetPlayerCamera().expired())
		{
			camera = actorM->GetPlayerCamera();
		}
	}
	return camera;
}

void Player::SearchTarget(Input& input, std::shared_ptr<PlayerCamera> camera, const std::list<std::shared_ptr<EnemyBase>>& enemys)
{
	//ターゲットがいなくなったら発見フラグをfalseにする
	if (m_targetInfo.m_pTarget.expired())
	{
		//前のフレームでは発見できていれば引き続き次の敵を探す
		if (m_targetInfo.m_isFound)
		{
			//他の敵を探す
			LockOnNearestEnemy(camera, enemys);
		}
		else
		{
			//解除
			ResetTarget(camera);
		}
	}
	else
	{
		//ターゲット
		auto target = std::dynamic_pointer_cast<EnemyBase>(m_targetInfo.m_pTarget.lock());

		//アクティブじゃないなら解除または死亡しているなら解除
		if (!target->IsActive() || target->GetCharaStatus()->IsDead())
		{
			//他の敵を探す
			LockOnNearestEnemy(camera, enemys);
		}
		else
		{
			//ターゲットがいるなら距離を確認
			Vector3 toTarget = target->GetNextPos() - GetPos();
			float distance = toTarget.Magnitude();
			if (distance > m_charaStatus->GetSearchRange())
			{
				//解除
				ResetTarget(camera);
			}
		}
	}

	//LBでロックオン開始と解除
	if (input.IsTrigger("LB"))
	{
		if (camera->IsLockOn())
		{
			//解除
			ResetTarget(camera);
		}
		else
		{
			//ロックオン
			LockOnNearestEnemy(camera, enemys);
		}
	}

	//ロックオン中のターゲット切り替え
	if (camera->IsLockOn())
	{
		//右スティックが倒されたら
		if (input.GetStickInfo().IsRightStickTrigger())
		{
			float rs = input.GetStickInfo().rightStickX;
			//現在のターゲット
			auto currentTarget = std::dynamic_pointer_cast<EnemyBase>(m_targetInfo.m_pTarget.lock());
			if (!currentTarget) return;

			std::shared_ptr<EnemyBase> newTarget = FindTargetInDirection(rs > 0.0f, camera, currentTarget, enemys);
			if (newTarget)
			{
				camera->StartLockOn(newTarget);
				m_targetInfo.m_pTarget = newTarget;
			}
		}
	}
}

void Player::LockOnNearestEnemy(std::shared_ptr<PlayerCamera> camera, const std::list<std::shared_ptr<EnemyBase>>& enemys)
{
	Vector3 playerPos = GetPos();
	std::shared_ptr<EnemyBase> nearestEnemy = nullptr;
	float minDis = m_charaStatus->GetSearchRange();

	//間に壁があるかをチェックする
	auto& physics = Physics::GetInstance();

	for (auto enemy : enemys)
	{
		if (!enemy->IsActive()) continue;
		if (enemy->GetCharaStatus()->IsDead()) continue;
		Vector3 dir = (enemy->GetLockOnViewPos() - playerPos);
		auto colls = physics.RayCast(playerPos, enemy->GetLockOnViewPos());

		//間に壁や床があるかをチェック
		bool isColl = false;
		for (auto coll : colls)
		{
			if (coll.expired())continue;
			//間に壁や床があるなら無視
			if (coll.lock()->GetGameTag() == GameTag::Object)
			{
				isColl = true;
				break;
			}
		}
		//何も間にないならチェック
		if (!isColl)
		{
			float distance = dir.Magnitude();
			if (distance < minDis)
			{
				minDis = distance;
				nearestEnemy = enemy;
			}
		}
	}

	if (nearestEnemy)
	{
		camera->StartLockOn(nearestEnemy);
		m_targetInfo.m_pTarget = nearestEnemy;
		m_targetInfo.m_isFound = true;
	}
	else
	{
		//解除
		ResetTarget(camera);
	}
}


std::shared_ptr<EnemyBase> Player::FindTargetInDirection(bool rightDir, std::shared_ptr<PlayerCamera> camera, 
	std::shared_ptr<EnemyBase> currentTarget, const std::list<std::shared_ptr<EnemyBase>>& enemys)
{
	Vector3 cameraPos = camera->GetPos();
	Vector3 forward = camera->GetForward();	//カメラ前方
	Vector3 right = camera->GetRight();     //カメラ右方向

	std::shared_ptr<EnemyBase> bestTarget;
	float minAngle = FLT_MAX;

	//間に壁があるかをチェックする
	auto& physics = Physics::GetInstance();

	for (auto enemy : enemys)
	{
		//活動していないまたは現在のターゲットなら無視
		if (!enemy->IsActive() || enemy == currentTarget) continue;

		//レイキャスト
		auto colls = physics.RayCast(cameraPos, enemy->GetNextPos());

		//間に壁や床があるかをチェック
		bool isColl = false;
		for (auto coll : colls)
		{
			if (coll.expired())continue;
			//間に壁や床があるなら無視
			if (coll.lock()->GetGameTag() == GameTag::Object)
			{
				isColl = true;
				break;
			}
		}
		//何も間にないならチェック
		if (!isColl)
		{
			Vector3 toEnemy = enemy->GetNextPos() - cameraPos;
			if (toEnemy.SqMagnitude() > 0.0f)
			{
				toEnemy = toEnemy.Normalize();
			}

			//右方向ベクトルとの角度を計算
			float dot = toEnemy.Dot(right);
			//右方向かチェック
			bool isRight = dot > 0.0f;

			//右スティックの方向と一致する側だけ見る
			if (rightDir != isRight) continue;

			//角度の絶対値が小さいほど前方向に近い
			float angle = std::acos(toEnemy.Dot(forward));
			if (angle < minAngle)
			{
				minAngle = angle;
				bestTarget = enemy;
			}
		}
	}
	return bestTarget;
}

void Player::ResetTarget(std::shared_ptr<PlayerCamera> camera)
{
	m_targetInfo.m_isFound = false;
	if (camera->IsLockOn())
	{
		//解除
		camera->EndLockOn();
	}
}

void Player::UpdateHit()
{
	//グリッジ
	auto& app = Application::GetInstance();
	auto& postEff = app.GetPostProcess();

	//ダメージを受けたら
	if (m_charaStatus->IsDamaged() && !m_isJustAvoided)
	{
		//グリッジ
		postEff->AddPostEffectState(ShaderPostProcess::PostEffectState::Glitch);

		//初期化
		m_glitchFrame = kHitGlitchFrame;
		m_glitchScale = kHitGlitchScale;
		m_glitchSpeed = kHitGlitchSpeed;
		m_glitchkStrengt = kHitGlitchStrengt;
		if(m_charaStatus->IsPinchHP())
		{
			m_glitchFrame *= kPinchRate;
			m_glitchScale *= kPinchRate;
			m_glitchSpeed *= kPinchRate;
			m_glitchkStrengt *= kPinchRate;
		}

		//攻撃を喰らった
		m_isHitGlitch = true;

		//カウントリセット
		m_glitchCountFrame = 0.0f;
	}

	//カウント
	m_glitchCountFrame += GetTimeScale();

	//攻撃を喰らった際の処理
	if (m_isHitGlitch)
	{
		//終了フレーム
		if (m_glitchCountFrame >= m_glitchFrame || m_isJustAvoided)
		{
			//グリッジを削除
			postEff->SubPostEffectState(ShaderPostProcess::PostEffectState::Glitch);
			//リセット
			m_isHitGlitch = false;
		}
		else
		{
			//グリッジフレーム間の処理
			//だんだん弱く
			float rate = m_glitchCountFrame / m_glitchFrame;
			postEff->SetBlockScele(MathSub::Lerp(m_glitchScale, 0.0f, rate));
			postEff->SetNoiseSpeed(MathSub::Lerp(m_glitchSpeed, 0.0f, rate));
			postEff->SetShakeStrength(MathSub::Lerp(m_glitchkStrengt, 0.0f, rate));
		}
	}
	else
	{
		//死亡
		if (m_isDelete)
		{
			//グレーに
			postEff->AddPostEffectState(ShaderPostProcess::PostEffectState::Gray);
			//グリッジ
			postEff->AddPostEffectState(ShaderPostProcess::PostEffectState::Glitch);
			//揺れる
			postEff->SetShakeStrength(m_glitchScale);
			postEff->SetBlockScele(m_glitchSpeed);
			postEff->SetNoiseSpeed(m_glitchkStrengt);

			++m_glitchScale;
			++m_glitchSpeed;
			++m_glitchkStrengt;
		}
		//ピンチ(定期的に画面が揺れる)
		else if (m_charaStatus->IsPinchHP())
		{
			//グレーに
			postEff->AddPostEffectState(ShaderPostProcess::PostEffectState::Gray);
			//グリッジ
			postEff->AddPostEffectState(ShaderPostProcess::PostEffectState::Glitch);
			if (m_glitchCountFrame >= kGlitchEndFrame)
			{
				m_glitchCountFrame = 0.0f;
			}
			else if (m_glitchCountFrame >= kGlitchStartFrame)
			{
				//揺れる
				postEff->SetShakeStrength(kHitGlitchStrengt);
				postEff->SetBlockScele(kHitGlitchScale);
				postEff->SetNoiseSpeed(kHitGlitchSpeed);
			}
			else
			{
				postEff->SetShakeStrength(0.0f);
				postEff->SetBlockScele(0.0f);
				postEff->SetNoiseSpeed(0.0f);
			}
		}
		//通常
		else
		{
			m_glitchCountFrame = 0.0f;
		}
	}
}

void Player::UpdatePutAwayWeapon()
{
	//武器を収める
	if (m_putAwayCountFrame <= 0.0f)
	{
		m_putAwayCountFrame = 0.0f;
		PutAwaySword();
	}
	else
	{
		//カウント
		m_putAwayCountFrame -= GetTimeScale();
	}
}

bool Player::IsWait()const
{
	return m_state->IsWait();
}