#include "Camera.h"
#include"Player.h"
#include"Input.h"
#include"GameDefine.h"
#include<DxLib.h>
#include<algorithm>

using namespace GameDefine;

Camera::Camera() :
	_cameraYaw(0.0f),
	_cameraPitch(0.5f),
	_cameraPos(VGet(0.0f, 0.0f, 0.0f)),
	_targetPos(VGet(0.0f, 0.0f, 0.0f)),
	_smoothedForward(VGet(0.0f, 0.0f, 0.0f))
{

}

Camera::~Camera()
{

}

void Camera::Init()
{

}

void Camera::Update(std::shared_ptr<Player> pPlayer, std::shared_ptr<Input> pInput)
{
	// 注視点
	VECTOR targetPos = VGet(0.0f, 0.0f, 0.0f);

	// プレイヤーが存在していることを確認してから、注視点をプレイヤーの座標に
	if (pPlayer != nullptr)
	{
		targetPos = pPlayer->GetPos();
		targetPos.y += CAMERA_TARGET_HEIGHT;


		// プレイヤーの向いている角度を保存し、補整前の向きを取得
		float playerAngle = pPlayer->GetAngle();
		VECTOR rowForward = VGet(sinf(playerAngle), 0, cosf(playerAngle));

		// 線形補完用の計算
		_smoothedForward = VAdd
		(
			VScale(_smoothedForward, 1.0f - CAMERA_FORWARD_LERP_RATE),
			VScale(rowForward, CAMERA_FORWARD_LERP_RATE)
		);

		// targetPosに反映
		targetPos = VAdd(targetPos, VScale(_smoothedForward, CAMERA_TARGET_FORWARD_OFFSET));
	}
	// カメラ位置
	VECTOR cameraPos = targetPos;

	// 入力に応じて水平、垂直方向にカメラの回転角度を決定
	_cameraYaw += CAMERA_YAW_SPEED * std::clamp(static_cast<float>(pInput->GetRightStickX()), -1.0f, 1.0f);
	_cameraPitch += -CAMERA_PITCH_SPEED * std::clamp(static_cast<float> (pInput->GetRightStickY()), -1.0f, 1.0f);

	// カメラの垂直方向の回転限界
	_cameraPitch = std::clamp(_cameraPitch, -CAMERA_PITCH_LIMIT, -0.4f);

	// カメラの回転方向を決定
	float x = cameraPos.x + CAMERA_DISTANCE * cosf(_cameraPitch) * sinf(_cameraYaw);
	float y = cameraPos.y + CAMERA_DISTANCE * sinf(_cameraPitch);
	float z = cameraPos.z + CAMERA_DISTANCE * cosf(_cameraPitch) * cosf(_cameraYaw);

	// 実際に代入
	cameraPos.x = x;
	cameraPos.y = y;
	cameraPos.z = z;

	_cameraPos = cameraPos;
	_targetPos = targetPos;
}

void Camera::Draw(int playerNum)
{
	switch (playerNum)
	{
	case 1:
		SetDrawArea(0, 0, WIDTH / 2, HEIGHT);
		SetCameraScreenCenter(WIDTH / 4, HEIGHT);
		SetupCamera_Perspective(DX_PI_F / 4.0f);
		SetCameraPositionAndTargetAndUpVec(_cameraPos, _targetPos, VGet(0.0f, 1.0f, 0.0f));
		break;
	case 2:
		SetDrawArea(WIDTH / 2, 0, WIDTH, HEIGHT);
		SetCameraScreenCenter((WIDTH * 3) / 4, HEIGHT);
		SetupCamera_Perspective(DX_PI_F / 4.0f);
		SetCameraPositionAndTargetAndUpVec(_cameraPos, _targetPos, VGet(0.0f, 1.0f, 0.0f));
		break;
	default:
		break;
	}
}
