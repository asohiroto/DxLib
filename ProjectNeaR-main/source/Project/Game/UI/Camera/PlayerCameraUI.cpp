#include "PlayerCameraUI.h"
#include "../../../General/AssetManager.h"
#include "../../../General/Game.h"
#include "../../Actor/Character/Enemy/EnemyBase.h"
#include "../../Camera/PlayerCamera.h"

namespace
{
	//ロックオンしていないときのバーの位置
	constexpr float kLockOffLeftVarPosX = -100.0f;
	constexpr float kLockOffRightVarPosX = Game::kScreenWidth + 100.0f;
	//ロックオンしてる時のバーの位置
	constexpr float kLockOnLeftVarPosX = Game::kScreenCenterX - 450.0f;
	constexpr float kLockOnRightVarPosX = Game::kScreenCenterX + 450.0f;
	//lerp
	constexpr float kLerpRate = 0.1f;
	//バーの動かす量
	constexpr float kHorizonMoveValue = 20.0f;
	//計算しない内積量
	constexpr float kThresholdDot = 0.2f;

	//縦方向の移動
	constexpr float kVerticalMoveValue = 50.0f;
	//バーのアングル
	constexpr float kMoveVarAngle = 3.0f;

	//ハンドル
	const std::wstring kLoclOnSideUIPath = L"Camera/LockOnSideUI";
	const std::wstring kLockOnCircleUIPath = L"Camera/LockOnCircleSmall";
}

PlayerCameraUI::PlayerCameraUI(std::weak_ptr<PlayerCamera> pCamera):
	m_pTarget(),
	m_isLockOn(false),
	m_pCamera(pCamera),
	m_beforeCameraDir()
{
	//バー
	int varHandle = AssetManager::GetInstance().GetImageHandle(kLoclOnSideUIPath);
	m_leftVar.handle = varHandle;
	m_rightVar.handle = varHandle;
	m_leftVar.pos.x = kLockOffLeftVarPosX;
	m_leftVar.pos.y = Game::kScreenCenterY;
	m_rightVar.pos.x = kLockOffRightVarPosX;
	m_rightVar.pos.y = Game::kScreenCenterY;
	//サークル
	int smallCircleHandle = AssetManager::GetInstance().GetImageHandle(kLockOnCircleUIPath);
	m_lockOnCircle.handle = smallCircleHandle;
	m_lockOnCircle.pos = Vector2(Game::kScreenCenterX,Game::kScreenCenterY);
	//向き
	if (pCamera.expired())return;
	m_beforeCameraDir = pCamera.lock()->GetLook();
}

PlayerCameraUI::~PlayerCameraUI()
{
}

void PlayerCameraUI::Update()
{
	if (m_isLockOn)
	{
		if (m_pTarget.expired())return;
		auto target = m_pTarget.lock();

		 //座標
		 Vector3 targetPos = target->GetLockOnViewPos();
		 Vector3 screenTargetPos = ConvWorldPosToScreenPos(targetPos.ToDxLibVector());
		 m_lockOnCircle.pos = screenTargetPos.XY();

		//回転
		m_lockOnCircle.angle += 0.01;

		//カメラの向きの変化からサイドのバーの動きを決める
		if (m_pCamera.expired())return;
		auto camera = m_pCamera.lock();
		//向き
		Vector3 cameraDir = camera->GetLook();
		if (cameraDir.SqMagnitude() > 0.0f)
		{
			cameraDir = cameraDir.Normalize();
		}

		//カメラから見て右を向いたか左を向いたかを計算
		Vector3 moveDir = cameraDir - m_beforeCameraDir;
		Vector3 moveHorizon = moveDir;
		moveHorizon.y = 0.0f;
		if (moveHorizon.SqMagnitude() > 0.0f)
		{
			moveHorizon = moveHorizon.Normalize();
		}
		//内積
		float HorizonDot = moveHorizon.Dot(camera->GetRight());
		if (!(HorizonDot > kThresholdDot) && !(HorizonDot < -kThresholdDot)) {
			//ほとんど前後（または微小）
			HorizonDot = 0.0f;
		}
		//水平方向の移動量
		HorizonDot *= kHorizonMoveValue;

		//縦方向の移動
		float verticalValue = moveDir.y;

		//上向きなら
		float leftAngle = 0.0f;
		float rightAngle = 0.0f;
		if (verticalValue > 0.0f)
		{
			verticalValue = -kVerticalMoveValue;
			//上を開く
			leftAngle = -kMoveVarAngle;
			rightAngle = kMoveVarAngle;
		}
		//下向きなら
		else if (verticalValue < 0.0f)
		{
			verticalValue = kVerticalMoveValue;
			//下を開く
			leftAngle = kMoveVarAngle;
			rightAngle = -kMoveVarAngle;
		}
		//少し傾ける
		m_leftVar.angle = static_cast<double>(MathSub::Lerp(m_leftVar.angle, MyMath::DEG_2_RAD * leftAngle, kLerpRate));
		m_rightVar.angle = static_cast<double>(MathSub::Lerp(m_rightVar.angle, MyMath::DEG_2_RAD * rightAngle, kLerpRate));

		//移動
		m_leftVar.pos.x = MathSub::Lerp(m_leftVar.pos.x, kLockOnLeftVarPosX + HorizonDot, kLerpRate);
		m_rightVar.pos.x = MathSub::Lerp(m_rightVar.pos.x, kLockOnRightVarPosX + HorizonDot, kLerpRate);
		m_leftVar.pos.y = MathSub::Lerp(m_leftVar.pos.y, Game::kScreenCenterY + verticalValue, kLerpRate);
		m_rightVar.pos.y = MathSub::Lerp(m_rightVar.pos.y, Game::kScreenCenterY + verticalValue, kLerpRate);

		//古い向きの更新
		m_beforeCameraDir = cameraDir;
	}
	else
	{
		m_leftVar.pos.x = MathSub::Lerp(m_leftVar.pos.x, kLockOffLeftVarPosX, kLerpRate);
		m_rightVar.pos.x = MathSub::Lerp(m_rightVar.pos.x, kLockOffRightVarPosX, kLerpRate);
	}
}

void PlayerCameraUI::Draw() const
{
	if (!m_isDraw)return;
	//両サイドのバー
	DrawRotaGraphF(m_leftVar.pos.x, m_leftVar.pos.y, 1.0, m_leftVar.angle, m_leftVar.handle, true);
	DrawRotaGraphF(m_rightVar.pos.x, m_rightVar.pos.y, 1.0, m_rightVar.angle, m_rightVar.handle, true);
	//サークル
	if (m_isLockOn)
	{
		DrawRotaGraphF(m_lockOnCircle.pos.x, m_lockOnCircle.pos.y,1.0, m_lockOnCircle.angle, m_lockOnCircle.handle, true);
	}
}

void PlayerCameraUI::EnableLockOn(std::weak_ptr<EnemyBase> target)
{
	if (target.expired())return;
	m_isLockOn = true;
	m_pTarget = target;
}

void PlayerCameraUI::DisableLockOn()
{
	m_isLockOn = false;
	m_pTarget.reset();
}
