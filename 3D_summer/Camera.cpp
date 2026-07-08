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
	_smoothedForward(VGet(0.0f, 0.0f, 0.0f)),
	_cameraLerpRate(0),
	_changedCameraMode(false)
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
	_cameraLerpRate = 0;

	// 注視点
	VECTOR targetPos = VGet(0.0f, 0.0f, 0.0f);

	if (pInput->IsTrigger(PAD_INPUT_B))
	{
		if (_changedCameraMode)
		{
			_changedCameraMode = false;
		}
		else if (!_changedCameraMode)
		{
			_changedCameraMode = true;
		}
	}

	// プレイヤーが存在していることを確認してから、注視点をプレイヤーの座標に
	if (pPlayer != nullptr)
	{
		targetPos = pPlayer->GetPos();

		// カメラモードによって切り替え
		if (_changedCameraMode)
		{
			targetPos.y += CAMERA_TARGET_HEIGHT;


			// プレイヤーの向いている角度を保存し、補整前の向きを取得
			float playerAngle = pPlayer->GetAngle();
			VECTOR rowForward = VGet(sinf(playerAngle), 0, cosf(playerAngle));

			if (pInput->GetLeftStickX() != 0 || pInput->GetLeftStickY() != 0)
			{
				_cameraLerpRate = 0.03f;
			}

			// 線形補完用の計算
			_smoothedForward = VAdd
			(
				VScale(_smoothedForward, 1.0f - _cameraLerpRate),
				VScale(rowForward, _cameraLerpRate)
			);

			// targetPosに反映
			targetPos = VAdd(targetPos, VScale(_smoothedForward, CAMERA_TARGET_FORWARD_OFFSET));
		}
	}

	// 入力に応じて水平、垂直方向にカメラの回転角度を決定
	_cameraYaw += CAMERA_YAW_SPEED * std::clamp(static_cast<float>(pInput->GetRightStickX()), -1.0f, 1.0f);
	_cameraPitch += -CAMERA_PITCH_SPEED * std::clamp(static_cast<float> (pInput->GetRightStickY()), -1.0f, 1.0f);

	// カメラの垂直方向の回転限界
	_cameraPitch = std::clamp(_cameraPitch, -CAMERA_PITCH_LIMIT, -0.4f);

	VECTOR cameraPos = VGet(0.0f, 0.0f, 0.0f);

	// カメラの回転方向を決定
	cameraPos.x = targetPos.x + CAMERA_DISTANCE * cosf(_cameraPitch) * sinf(_cameraYaw);
	cameraPos.y = targetPos.y + CAMERA_DISTANCE * sinf(_cameraPitch);
	cameraPos.z = targetPos.z + CAMERA_DISTANCE * cosf(_cameraPitch) * cosf(_cameraYaw);

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

	DrawFormatString(3, 100, 0xffffff, "CameraMode : %d", _changedCameraMode);
}
