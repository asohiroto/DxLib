#include "EnemyStateDeath.h"
#include "EnemyBase.h"
#include "../../../../General/Model.h"
#include "../../../../General/Input.h"
#include "../../../../General/Collision/Rigidbody.h"
#include "../../../../General/CharaStatus.h"
#include "../../../../General/Effect/EffekseerManager.h"
#include "../../../../General/Sound/SoundManager.h"
#include "../../../Camera/CameraController.h"
#include "../../../Camera/BossCamera.h"

namespace
{
	//アニメーション
	const std::wstring kDeath = L"Death";
	//エフェクト
	const std::wstring kDeadEff = L"Dead";
	//SE
	const std::wstring kDeadSE = L"Dead";
	const std::wstring kKOSE = L"KO";
	//ボイス
	const std::wstring kDeadVoice = L"Dead";
	//ふっとばす力
	constexpr float kMinSmashPower = 10.0f;
	constexpr float kMaxSmashPower = 20.0f;
	//死亡までのフレーム
	constexpr int kDeadFrame = 20;

	//カメラ角度
	constexpr float kCameraAngle = 30.0f * MyMath::DEG_2_RAD;
}

EnemyStateDeath::EnemyStateDeath(std::weak_ptr<Actor> enemy, bool isWait) :
	EnemyStateBase(enemy,isWait)
{
	if (m_pOwner.expired())return;
	auto owner = std::dynamic_pointer_cast<EnemyBase>(m_pOwner.lock());
	auto model = owner->GetModel();
	model->SetAnim(owner->GetAnim(kDeath).c_str(), false);
	owner->SetCollState(CollisionState::Dead);

	auto& soundManager = SoundManager::GetInstance();

	//ボスではないなら
	if (!owner->IsBoss())
	{
		//ふっとばされる
		owner->GetRb()->SetVecY(MyMath::GetRandF(kMinSmashPower, kMaxSmashPower));
		owner->GetRb()->AddVec(owner->GetToTargetVec() * -MyMath::GetRandF(kMinSmashPower, kMaxSmashPower));
	}
	else
	{
		//とどめSE
		soundManager.PlayOnceSE(owner->GetSEPath(kKOSE));

		owner->GetRb()->SetMoveVec(Vector3::Zero());

		Vector3 dir = model->GetDir();
		if (owner->GetTargetInfo().m_isFound)
		{
			dir = owner->GetToTargetVec();
			dir.y = 0.0f;
			if (dir.SqMagnitude() > 0.0f)
			{
				dir = dir.Normalize();
			}
		}
		dir = Quaternion::AngleAxis(kCameraAngle, dir.Cross(Vector3::Up())) * dir;
		//カメラの作成
		CameraController::GetInstance().PushCamera(std::make_shared<BossCamera>(owner->GetCenterPos(), dir,
			(owner->GetHeadPos() - owner->GetPos()).Magnitude(), owner->GetActorManager(), false));
	}

	//ボイス再生
	soundManager.PlayVoice(owner->GetVoicePath(kDeadVoice));

	//無敵に
	owner->GetCharaStatus()->SetIsNoDamage(true);
}

EnemyStateDeath::~EnemyStateDeath()
{
}

void EnemyStateDeath::Init()
{
	//次の状態を自分の状態を入れる
	ChangeState(shared_from_this());
}

void EnemyStateDeath::Update()
{
	if (m_pOwner.expired())return;
	auto owner = std::dynamic_pointer_cast<EnemyBase>(m_pOwner.lock());

	++m_frame;
	//モデルのアニメーションが終わったら
	if ((m_frame > kDeadFrame && !owner->IsBoss()) ||
		owner->GetModel()->IsFinishAnim())
	{
		if (!owner->IsDelete())
		{
			//爆発
			EffekseerManager::GetInstance().CreateEffect(owner->GetEffectPath(kDeadEff), owner->GetPos());

			//爆発SE
			SoundManager::GetInstance().PlayOnceSE(owner->GetSEPath(kDeadSE));
		}
		owner->Delete();
	}
}
