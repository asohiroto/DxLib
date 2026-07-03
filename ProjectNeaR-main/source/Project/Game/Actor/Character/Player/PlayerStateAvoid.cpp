#include "PlayerStateAvoid.h"
#include "PlayerStateIdle.h"
#include "Player.h"
#include "PlayerStateMoving.h"
#include "PlayerStateLightAttack.h"
#include "PlayerStateHeavyAttack.h"
#include "PlayerStateDeath.h"
#include "PlayerStateHit.h"
#include "../../../../General/Input.h"
#include "../../../../General/Model.h"
#include "../../../../General/Collision/Rigidbody.h"
#include "../../../../General/CharaStatus.h"
#include "../../../../Main/Application.h"
#include "../../../../General/ShaderPostProcess.h"
#include "../../../../General/Effect/EffekseerManager.h"
#include "../../../../General/Sound/SoundManager.h"
namespace
{
	//速度
	constexpr float kSpeedDif = 10.0f;
	//だんだん早くなっていく
	constexpr float kLerpSpeedRate = 0.1f;
	
	//アニメーション
	const std::wstring kForwardAvoid = L"AvoidForward";
	const std::wstring kBackAvoid = L"AvoidBack";
	const std::wstring kJustAvoid = L"JustAvoid";

	//SE
	const std::wstring kAvoidSE = L"JustAvoid";

	//ボイス
	const std::wstring kJustAvoidVoice1 = L"JustAvoid1";
	const std::wstring kJustAvoidVoice2 = L"JustAvoid2";

	//ジャスト回避フレーム
	constexpr float kJustFrame = 20.0f;
	//スロー速度
	constexpr float kSlowSpeed = 0.05f;
	//無敵フレーム
	constexpr float kNoDamageFrame = 15.0f;
	//ジャスト回避終了前
	constexpr float kFisishJustAvoidFrame = 5.0f;
	//終了可能フレーム
	constexpr float kEnableFinishAvoidFrame = kJustFrame + 5;
}

PlayerStateAvoid::PlayerStateAvoid(std::weak_ptr<Actor> player, bool isWait) :
	PlayerStateBase(player,isWait),
	m_avoidDir(),
	m_speed(0.0f),
	m_endSpeed(0.0f),
	m_isJustAvoid(false),
	m_finishJustAvoid(0.0f),
	m_isLightAttack(false),
	m_isHeavyAttack(false),
	m_isBack(false)
{
	if (m_pOwner.expired())return;
	auto owner = std::dynamic_pointer_cast<Player>(m_pOwner.lock());
	owner->SetCollState(CollisionState::Move);

	//武器は持たない
	owner->PutAwaySword();

	auto& input = Input::GetInstance();
	//移動
	Vector3 vec = Vector3::Zero();
	vec.x = static_cast<float>(input.GetStickInfo().leftStickX);
	vec.z = -static_cast<float>(input.GetStickInfo().leftStickY);
	//下がる
	bool isBack = false;
	//移動方向があるなら
	if (vec.SqMagnitude() > 0.0f)
	{
		vec = vec.Normalize();
	}
	//ない場合は下がる
	else
	{
		isBack = true;
		vec = Vector3::Back();
	}
	m_isBack = isBack;
	//カメラの向きに合わせて移動方向を変える
	vec = owner->GetCameraRot() * vec;
	//回避方向設定
	m_avoidDir = vec;

	//前進か下がるかでアニメーション切り替え
	if (!isBack)
	{
		owner->GetModel()->SetAnim(owner->GetAnim(kForwardAvoid).c_str(), false);
		//モデルの向き
		owner->GetModel()->SetDir(vec.XZ());
	}
	else
	{
		owner->GetModel()->SetAnim(owner->GetAnim(kBackAvoid).c_str(), false);
		//モデルの向き
		owner->GetModel()->SetDir(vec.XZ() * -1);
	}

	auto status = owner->GetCharaStatus();
	//速度
	float baseSpeed = status->GetMS();
	m_endSpeed = baseSpeed + kSpeedDif;
	float startSpeed = MathSub::ClampFloat(baseSpeed - kSpeedDif,0, m_endSpeed);
	m_speed = startSpeed;

	//空中にいるなら重力の影響を受けない
	if (!owner->IsFloor())
	{
		auto rb = owner->GetRb();
		rb->SetIsGravity(false);
		rb->SetVecY(0.0f);
		//回避不可能(ジャンプをするか地面に付くと可能に)
		owner->SetIsAvoidable(false);
	}

	//無敵
	status->SetIsNoDamage(true);

	//SE再生
	SoundManager::GetInstance().PlayOnceSE(owner->GetSEPath(kAvoidSE));
}

PlayerStateAvoid::~PlayerStateAvoid()
{
	if (m_pOwner.expired())return;

	auto owner = std::dynamic_pointer_cast<Player>(m_pOwner.lock());
	owner->GetRb()->SetIsGravity(true);

	//ジャスト回避に成功しているなら数フレーム間無敵
	if (m_isJustAvoid)
	{
		//無敵
		owner->SetNoDamageFrame(kNoDamageFrame);
	}

	
}

void PlayerStateAvoid::Init()
{
	//次の状態を自分の状態を入れる
	ChangeState(shared_from_this());
}

void PlayerStateAvoid::Update()
{
	if (m_pOwner.expired())return;
	auto owner = std::dynamic_pointer_cast<Player>(m_pOwner.lock());
	auto model = owner->GetModel();
	auto& app = Application::GetInstance();

	//入力
	auto& input = Input::GetInstance();

	//フレームカウント
	CountFrame();

	//ジャスト回避
	UpdateJustAvoid(owner, model, app);

	//強制待機
	if (m_isWait)
	{
		//待機
		ChangeState(std::make_shared<PlayerStateIdle>(m_pOwner, m_isWait));
		return;
	}

	//死亡
	if (owner->GetCharaStatus()->IsDead())
	{
		ChangeState(std::make_shared<PlayerStateDeath>(m_pOwner, m_isWait));
		return;
	}
	//やられ
	if (owner->GetCharaStatus()->IsHitReaction() && !m_isJustAvoid)
	{
		ChangeState(std::make_shared<PlayerStateHit>(m_pOwner, m_isWait));
		return;
	}


	//ジャスト回避成功後
	if (m_isJustAvoid)
	{
		if (input.IsBuffered("X"))
		{
			m_isLightAttack = true;
			m_isHeavyAttack = false;
		}
		if (input.IsBuffered("Y"))
		{
			m_isLightAttack = false;
			m_isHeavyAttack = true;
		}
	}
	else
	{
		//入力がないかつジャスト回避成功していない場合即終了
		if (!input.IsPress("B") && (m_frame > kEnableFinishAvoidFrame))
		{
			//無敵解除
			owner->GetCharaStatus()->SetIsNoDamage(false);
			if (input.GetStickInfo().IsLeftStickInput())
			{
				//走る
				ChangeState(std::make_shared<PlayerStateMoving>(m_pOwner, m_isWait, true));
				return;
			}
			else
			{
				//待機
				ChangeState(std::make_shared<PlayerStateIdle>(m_pOwner, m_isWait));
				return;
			}
			return;
		}
	}

	if (model->IsFinishAnim())
	{
		//攻撃
		if (m_isLightAttack)
		{
			ChangeState(std::make_shared<PlayerStateLightAttack>(m_pOwner, m_isWait, false, true));
			return;
		}
		else if(m_isHeavyAttack)
		{
			ChangeState(std::make_shared<PlayerStateHeavyAttack>(m_pOwner, m_isWait, false, true));
			return;
		}
		

		//無敵解除
		owner->GetCharaStatus()->SetIsNoDamage(false);
		auto& input = Input::GetInstance();
		if (input.GetStickInfo().IsLeftStickInput())
		{
			//走る
			ChangeState(std::make_shared<PlayerStateMoving>(m_pOwner, m_isWait, true));
			return;
		}
		else
		{
			//待機
			ChangeState(std::make_shared<PlayerStateIdle>(m_pOwner, m_isWait));
			return;
		}
	}

	//回避移動
	MoveAvoid(input, app, owner);
}

void PlayerStateAvoid::InitJustAvoid(std::shared_ptr<Model> model, std::shared_ptr<Player> owner)
{
	//ジャスト回避成功
	m_isJustAvoid = true;

	//ジャスト回避
	model->SetAnim(owner->GetAnim(kJustAvoid).c_str(), false, 1.0f);

	//終了フレーム
	m_finishJustAvoid = model->GetTotalAnimFrame() - kFisishJustAvoidFrame;

	//スローモーション
	auto& app = Application::GetInstance();
	app.SetTimeScale(kSlowSpeed);
	//自分のタイムスケールを使う
	owner->EnableIsMyScale();
	owner->SetTimeScale(1.0f);

	//シェーダー
	app.GetPostProcess()->SetJustAvoidEffectTime(model->GetTotalAnimFrame());
	app.GetPostProcess()->AddPostEffectState(ShaderPostProcess::PostEffectState::JustAvoid);

	//フレームリセット
	m_frame = 0.0f;

	auto status = owner->GetCharaStatus();
	//無敵
	status->SetIsNoDamage(true);

	//攻撃をなかったことに
	status->ResetDamage();

	//エフェクト
	EffekseerManager::GetInstance().CreateTrackActorEffect(owner->GetEffectPath(kJustAvoid), owner);

	//プレイヤーを非表示に
	owner->SetIsDraw(false);

	//ジャスト回避数カウント
	owner->AddJustAvoidNum();

	auto& soundManager = SoundManager::GetInstance();
	//SE再生
	soundManager.PlayOnceSE(owner->GetSEPath(kAvoidSE));

	//攻撃ボイス
	auto voicePath = kJustAvoidVoice1;
	if (MyMath::IsRand())voicePath = kJustAvoidVoice2;
	soundManager.PlayVoice(owner->GetVoicePath(voicePath));

}

void PlayerStateAvoid::UpdateJustAvoid(std::shared_ptr<Player> owner, std::shared_ptr<Model> model, Application& app)
{
	if (!m_isJustAvoid)
	{
		//ジャスト回避フレーム内で攻撃を受けたら
		if (m_frame <= kJustFrame && owner->GetCharaStatus()->IsHit())
		{
			//ジャスト回避成功時に初期化
			InitJustAvoid(model, owner);
		}
	}
	else
	{
		//終了フレーム
		if (m_finishJustAvoid < m_frame)
		{
			//元に戻す
			app.SetTimeScale(1.0f);
			//自分のタイムスケールを使わない
			owner->DisableIsMyScale();
			//描画する
			owner->SetIsDraw(true);
		}
		else
		{
			//スローモーション
			app.SetTimeScale(kSlowSpeed);
		}
	}
}

void PlayerStateAvoid::MoveAvoid(Input& input, Application& app, std::shared_ptr<Player> owner)
{
	bool isInput = input.GetStickInfo().IsLeftStickInput();
	//移動
	if (isInput)
	{
		//入力方向に変更
		m_avoidDir = InputMoveVec(owner, input);
		//モデルの向き
		Vector3 dir = m_avoidDir;
		if (m_isBack)
		{
			dir *= -1;
		}
		owner->GetModel()->SetDir(dir.XZ());
	}
	float timeScale = app.GetTimeScale();

	m_speed = MathSub::Lerp(m_speed, m_endSpeed, kLerpSpeedRate);
	Vector3 vec = m_avoidDir * m_speed * timeScale;
	//移動
	owner->GetRb()->SetMoveVec(vec);
}