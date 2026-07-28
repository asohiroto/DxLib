#include "Camera.h"
#include "Players/Player.h"
#include "Inputs/Input.h"
#include <algorithm>
#include <cmath>

namespace
{
	// 注視点の高さの補正値
	constexpr float TARGET_HEIGHT = 200.0f;
	// プレイヤーまでの距離
	constexpr float DISTANCE = -800.0f;
	// 水平方向の回転速度
	constexpr float YAW_SPEED = 0.03f;
	// 垂直方向の回転速度
	constexpr float PITCH_SPEED = 0.03f;
	// 線形補間度
	constexpr float LERP_RATE = 0.5f;
	// カメラの垂直方向の上限回転角度
	constexpr float PITCH_UP_LIMIT = -0.95f;
	// カメラの垂直方向の下限回転角度
	constexpr float PITCH_DOWN_LIMIT = -0.2f;
}

Camera::Camera() :
	_cameraYaw(0.0f),
	_cameraPitch(0.0f),
	_dispCameraYaw(0.0f),
	_dispCameraPitch(0.0f),
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

void Camera::Update(std::shared_ptr<Player> pPlayer, std::shared_ptr<Input> pInput)
{
	// 右スティックの入力を保存
	int rx = pInput->GetRightStickX();
	int ry = pInput->GetRightStickY();

	// 注視点を設定
	if (pPlayer != nullptr) _targetPos = pPlayer->GetPos();
	_targetPos.y += TARGET_HEIGHT;

	// 入力値を-1.0～1.0の値に変換
	_cameraYaw += YAW_SPEED * std::clamp(rx / 1000.0f, -1.0f, 1.0f);
	_cameraPitch += PITCH_SPEED * std::clamp(ry / 1000.0f, -1.0f, 1.0f);

	// カメラの垂直方向の回転角度に制限をかける
	_cameraPitch = std::clamp(_cameraPitch, PITCH_UP_LIMIT, PITCH_DOWN_LIMIT);

	// 表示用の回転角度に線形補間をかける
	_dispCameraYaw += (_cameraYaw - _dispCameraYaw) * LERP_RATE;
	_dispCameraPitch += (_cameraPitch - _dispCameraPitch) * LERP_RATE;

	VECTOR cameraPos = VGet(0.0f, 0.0f, 0.0f);
	cameraPos.x = DISTANCE * std::cosf(_dispCameraPitch) * std::sinf(_dispCameraYaw);
	cameraPos.y = DISTANCE * std::sinf(_dispCameraPitch);
	cameraPos.z = DISTANCE * std::cosf(_dispCameraPitch) * std::cosf(_dispCameraYaw);

	_cameraPos = VAdd(cameraPos, _targetPos);
}

void Camera::Draw()
{
	// カメラを設置
	SetCameraPositionAndTargetAndUpVec(_cameraPos, _targetPos, VGet(0.0f, 1.0f, 0.0f));
}
