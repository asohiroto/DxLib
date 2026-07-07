#include "Camera.h"
#include"Input.h"
#include"Player.h"
#include<algorithm>

namespace
{
	// 水平方向の速度
	constexpr float kCameraYawSpeed = 0.02f;
	// 垂直方向の速度
	constexpr float kCameraPitchSpeed = 0.02f;
	// 垂直方向の回転の制限
	constexpr float kCameraLimit = 1.5f;
}

void Camera::Init()
{

}

void Camera::Update(std::shared_ptr<Player> player)
{
	VECTOR targetPos = VGet(0.0f, 0.0f, 0.0f);

	if (player != nullptr)
	{
		targetPos = player->GetPos();
	}

	VECTOR cameraPos = targetPos;
	Input& input = Input::GetInstance();

	_cameraYaw += kCameraYawSpeed * std::clamp(static_cast<float>(input.GetRightStickX()), -1.0f, 1.0f);
	_cameraPitch += -kCameraPitchSpeed * std::clamp(static_cast<float>(input.GetRightStickY()), -1.0f, 1.0f);

	if (_cameraPitch > kCameraLimit) _cameraPitch = kCameraLimit;
	if (_cameraPitch < -kCameraLimit) _cameraPitch = -kCameraLimit;

	float x = cameraPos.x + _cameraDistance * cosf(_cameraPitch) * sinf(_cameraYaw);
	float y = cameraPos.y + _cameraDistance * sinf(_cameraPitch);
	float z = cameraPos.z + _cameraDistance * cosf(_cameraPitch) * cosf(_cameraYaw);

	cameraPos.x = x;
	cameraPos.y = y;
	cameraPos.z = z;

	SetCameraPositionAndTargetAndUpVec(cameraPos, targetPos, VGet(0.0f, 1.0f, 0.0f));
}