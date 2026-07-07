#include "Camera.h"
#include"Player.h"
#include"Input.h"
#include"GameDefine.h"
#include<DxLib.h>
#include<algorithm>

using namespace GameDefine;

Camera::Camera() :
	_cameraDistance(-1000.0f),
	_cameraYaw(0.0f),
	_cameraPitch(0.5f),
	p_Input(nullptr)
{

}

Camera::~Camera()
{

}

void Camera::Init()
{
	p_Input = std::make_shared<Input>();
}

void Camera::Update(std::shared_ptr<Player> pPlayer)
{
	p_Input->Update();
	// 注視点
	VECTOR targetPos = VGet(0.0f, 0.0f, 0.0f);

	// プレイヤーが存在していることを確認してから、注視点をプレイヤーの座標に
	if (pPlayer != nullptr)
	{
		targetPos = pPlayer->GetPos();
	}

	// カメラ位置
	VECTOR cameraPos = targetPos;

	// 入力に応じて水平、垂直方向にカメラの回転角度を決定
	_cameraYaw += CAMERA_YAW_SPEED * std::clamp(static_cast<float>(p_Input->GetRightStickX()), -1.0f, 1.0f);
	_cameraPitch += -CAMERA_PITCH_SPEED * std::clamp(static_cast<float> (p_Input->GetRightStickY()), -1.0f, 1.0f);
	// カメラの垂直方向の回転限界
	_cameraPitch = std::clamp(_cameraPitch, -CAMERA_PITCH_LIMIT, CAMERA_PITCH_LIMIT);

	// カメラの回転方向を決定
	float x = cameraPos.x + _cameraDistance * cosf(_cameraPitch) * sinf(_cameraYaw);
	float y = cameraPos.y + _cameraDistance * sinf(_cameraPitch);
	float z = cameraPos.z + _cameraDistance * cosf(_cameraPitch) * cosf(_cameraYaw);

	// 実際に代入
	cameraPos.x = x;
	cameraPos.y = y;
	cameraPos.z = z;

	SetCameraPositionAndTargetAndUpVec(cameraPos, targetPos, VGet(0.0f, 1.0f, 0.0f));
}
