#include "Camera.h"
#include "Player.h"

namespace
{
	// 注視点の高さの補正値
	constexpr float TARGET_HEIGHT = -150.0f;
	// プレイヤーまでの距離
	constexpr float DISTANCE = -1750.0f;
}

Camera::Camera() :
	_cameraYaw(0.0f),
	_cameraPitch(0.0f),
	_cameraPos(VGet(0.0f, 0.0f, 0.0f)),
	_targetPos(VGet(0.0f, 0.0f, 0.0f))
{
}

Camera::~Camera()
{
}

void Camera::Init()
{
}

void Camera::End()
{
}

void Camera::Update(std::shared_ptr<Player> pPlayer)
{
	// 注視点を設定
	if (pPlayer != nullptr) _targetPos = pPlayer->GetPos();
	_targetPos.y += TARGET_HEIGHT;

	_cameraPos = VAdd(_cameraPos, _targetPos);
}

void Camera::Draw()
{
	// カメラを設置
	SetCameraPositionAndTargetAndUpVec(_cameraPos, _targetPos, VGet(0.0f, 1.0f, 0.0f));
}
