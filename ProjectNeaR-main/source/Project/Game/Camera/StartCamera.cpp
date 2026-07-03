#include "StartCamera.h"
#include "CameraController.h"
#include "../Actor/Character/Player/Player.h"
#include "../../General/Model.h"
#include "../../General/Game.h"
#include "../../General/AssetManager.h"
#include "../Actor/ActorManager.h"

namespace
{
	//ニアファー
	constexpr float kNear = 10.0f;
	constexpr float kFar = 20000.0f;

	//角度
	constexpr float kFirstAngle = -70.0f;
	//距離
	constexpr float kFirstDistance = 60.0f;
	//Lerpの割合
	constexpr float kLerpRate = 0.5f;

	//第一フェーズ
	constexpr int kFirstPhaseFrame = 202;
	//横からのアングル
	constexpr float kSideAngle = 85.0f;
	//上昇速度
	constexpr float kHeightAdjustSpeed = 0.6f;
	//少し下を見る
	constexpr float kLookDownAdjust = -10.0f;
	

	//第二フェーズ
	constexpr int kSecondPhaseFrame = 260;
	//距離
	constexpr float kSecondDistance = 147.0f;
	//高さ
	constexpr float kSecondHeight = 95.0f;
	//少し上を見る
	constexpr float kLookUpAdjust = 10.0f;

	//第三フェーズ
	constexpr int kThirdPhaseFrame = 300;
	//距離
	constexpr float kThirdDistance = 400.0f;
	//後ろの角度
	constexpr float kBackAngle = 180.0f;
	//角速度
	constexpr float kBackRotaSpeed = 2.0f;

	//シネマスコープ
	const std::wstring kCinemaScopeImagePath = L"CinemaScope";

	//シネマスコープのY移動速度
	constexpr float kCinemaScopeMoveSpeed = 5.0f;
	//シネマスコープの画面外位置
	constexpr float kCinemaScopeOutTopPosY = -50.0f;
	constexpr float kCinemaScopeOutUnderPosY = Game::kScreenHeight + 50.0f;
	//シネマスコープの画面内位置
	constexpr float kCinemaScopeInTopPosY = 30.0f;
	constexpr float kCinemaScopeInUnderPosY = Game::kScreenHeight - 30.0f;
	//シネマスコープのlerp率
	constexpr float kCinemaScopeLerpRate = 0.05f;

}

StartCamera::StartCamera(std::weak_ptr<Player> pPlayer, std::weak_ptr<ActorManager> pActorManager):
	CameraBase(),
	m_pPlayer(pPlayer),
	m_rotaAngle(kFirstAngle),
	m_heightAdjust(0.0f),
	m_countFrame(0),
	m_cinemaScopeHandle(-1),
	m_cinemaScopeTopPosY(kCinemaScopeOutTopPosY),
	m_cinemaScopeUnderPosY(kCinemaScopeOutUnderPosY),
	m_pActorManager(pActorManager)
{
	
}

StartCamera::~StartCamera()
{
	//キャラクターを活動可能状態に
	if (m_pActorManager.expired())return;
	m_pActorManager.lock()->AllOperate();
}

void StartCamera::Init()
{
	//値の初期化
	m_distance = kFirstDistance;
	m_vertexAngle = 0.0f;
	m_front = Vector3::Forward();
	m_right = Vector3::Right();
	m_look = m_front;
	m_rotH = Quaternion::IdentityQ();
	m_viewPos = Vector3::Zero();
	m_cameraPos = Vector3::Zero();
	m_rotaAngle = kFirstAngle;
	m_heightAdjust = 0.0f;
	m_countFrame = 0;
	m_cinemaScopeHandle = AssetManager::GetInstance().GetImageHandle(kCinemaScopeImagePath);
	m_cinemaScopeTopPosY = kCinemaScopeOutTopPosY;
	m_cinemaScopeUnderPosY = kCinemaScopeOutUnderPosY;

	if (m_pPlayer.expired())return;

	//初期位置設定
	auto player = m_pPlayer.lock();
	m_look = player->GetPos();
	m_cameraPos = player->GetPos() + player->GetModel()->GetDir() * m_distance;

	//プレイヤーをスタート状態に
	player->SetStartState();

	//キャラクターを待機状態に
	if (m_pActorManager.expired())return;
	m_pActorManager.lock()->AllWait();
}

void StartCamera::Update()
{
	//レンダーターゲットが毎フレーム更新されるので
	//ニアクリップとファークリップを毎フレーム設定し直す
	SetCameraNearFar(kNear, kFar);

	if (m_pPlayer.expired())return;
	auto player = m_pPlayer.lock();

	++m_countFrame;

	if (m_countFrame <= kFirstPhaseFrame)
	{
		//画面内へ移動
		m_cinemaScopeUnderPosY = MathSub::Lerp(m_cinemaScopeUnderPosY, kCinemaScopeInUnderPosY, kCinemaScopeLerpRate);
		m_cinemaScopeTopPosY = MathSub::Lerp(m_cinemaScopeTopPosY, kCinemaScopeInTopPosY, kCinemaScopeLerpRate);

		++m_rotaAngle;
		if(m_rotaAngle > kSideAngle)
		{
			m_rotaAngle = kSideAngle;
		}
		else
		{
			m_heightAdjust += kHeightAdjustSpeed;
		}
		m_viewPos = player->GetPos();
		m_cameraPos = (Quaternion::AngleAxis(m_rotaAngle * MyMath::DEG_2_RAD, Vector3::Up()) * (player->GetModel()->GetDir() * m_distance)) + player->GetPos();
		m_cameraPos.y += m_heightAdjust;
		m_viewPos.y = m_cameraPos.y + kLookDownAdjust;
	}
	else if (m_countFrame <= kSecondPhaseFrame)
	{
		m_distance = MathSub::Lerp(m_distance, kSecondDistance, kLerpRate);
		m_heightAdjust = MathSub::Lerp(m_heightAdjust, kSecondHeight, kLerpRate);
		m_viewPos = player->GetPos();
		m_cameraPos = (Quaternion::AngleAxis(m_rotaAngle * MyMath::DEG_2_RAD, Vector3::Up()) * (player->GetModel()->GetDir() * m_distance)) + player->GetPos();
		m_cameraPos.y += m_heightAdjust;
		m_viewPos.y = m_cameraPos.y + kLookUpAdjust;
	}
	else if (m_countFrame <= kThirdPhaseFrame)
	{
		//画面外へ移動
		m_cinemaScopeUnderPosY = MathSub::Lerp(m_cinemaScopeUnderPosY, kCinemaScopeOutUnderPosY, kCinemaScopeLerpRate);
		m_cinemaScopeTopPosY = MathSub::Lerp(m_cinemaScopeTopPosY, kCinemaScopeOutTopPosY, kCinemaScopeLerpRate);

		m_distance = MathSub::Lerp(m_distance, kThirdDistance, kLerpRate);
		m_rotaAngle += kBackRotaSpeed;
		if (m_rotaAngle > kBackAngle)
		{
			m_rotaAngle = kBackAngle;
		}
		m_viewPos = player->GetPos();
		m_cameraPos = (Quaternion::AngleAxis(m_rotaAngle * MyMath::DEG_2_RAD, Vector3::Up()) * (player->GetModel()->GetDir() * m_distance)) + player->GetPos();
		m_cameraPos.y += m_heightAdjust;
		m_viewPos.y = m_cameraPos.y + kLookUpAdjust;
	}
	else
	{
		//反映 
		DxLib::SetCameraPositionAndTarget_UpVecY(
			m_cameraPos.ToDxLibVector(),
			m_viewPos.ToDxLibVector()
		);

		//削除
		auto& cameraController = CameraController::GetInstance();
		cameraController.PopCamera(m_cameraPos, m_viewPos);
		return;
	}
	//反映 
	DxLib::SetCameraPositionAndTarget_UpVecY(
		m_cameraPos.ToDxLibVector(),
		m_viewPos.ToDxLibVector()
	);
	
}

void StartCamera::Draw() const
{
	DrawRotaGraphF(Game::kScreenCenterX, m_cinemaScopeTopPosY, 1.0, 0.0, m_cinemaScopeHandle, true);
	DrawRotaGraphF(Game::kScreenCenterX, m_cinemaScopeUnderPosY, 1.0, 0.0, m_cinemaScopeHandle, true);
}
