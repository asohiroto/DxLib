#include "Camera.h"
#include"Player.h"
#include"Input.h"
#include"GameDefine.h"
#include<DxLib.h>
#include<algorithm>
#include<cmath>

using namespace GameDefine;

Camera::Camera() :
	_cameraYaw(0.0f),
	_cameraPitch(0.5f),
	_cameraPos(VGet(0.0f, 0.0f, 0.0f)),
	_targetPos(VGet(0.0f, 0.0f, 0.0f)),
	_smoothedForward(VGet(0.0f, 0.0f, 0.0f)),
	_cameraLerpRate(0),
	_changedCameraMode(false),
	_initialize(false),
	_nowDir(VGet(0.0f, 0.0f, 0.0f)),
	_slerpedDir(VGet(0.0f, 0.0f, 0.0f)),
	_isTest(false),
	_dispCameraYaw(0.0f),
	_dispCameraPitch(0.5f),
	_cameraDistance(0)
{

}

Camera::~Camera()
{

}

void Camera::Init()
{
	_cameraDistance = CAMERA_DISTANCE;
}

void Camera::Update(std::shared_ptr<Player> pPlayer, std::shared_ptr<Player> pOther, std::shared_ptr<Input> pInput)
{
	_cameraLerpRate = 0;

	int rx = pInput->GetRightStickX();
	int ry = pInput->GetRightStickY();

	// 注視点
	VECTOR targetPos = VGet(0.0f, 0.0f, 0.0f);

	// プレイヤーが存在していることを確認してから、注視点をプレイヤーの座標に
	if (pPlayer != nullptr)
	{
		targetPos = pPlayer->GetPos();

		//targetPos.y += CAMERA_TARGET_HEIGHT;

	}

	// 入力に応じて水平、垂直方向にカメラの回転角度を決定
	// （-1000～1000の入力値を-1.0～1.0に正規化して使う）
	_cameraYaw = _cameraYaw + CAMERA_YAW_SPEED * std::clamp(rx / 1000.0f, -1.0f, 1.0f);
	_cameraPitch = _cameraPitch + -CAMERA_PITCH_SPEED * std::clamp(ry / 1000.0f, -1.0f, 1.0f);

	// カメラの垂直方向の回転限界
	_cameraPitch = std::clamp(_cameraPitch, -CAMERA_PITCH_LIMIT, -0.2f);

	// カメラの回転角に線形補間をかける
	_dispCameraYaw += (_cameraYaw - _dispCameraYaw) * CAMERA_LERP_RATE;
	_dispCameraPitch += (_cameraPitch - _dispCameraPitch) * CAMERA_LERP_RATE;

	// 回転角度から求めた、カメラの目標位置
	VECTOR cameraPos = VGet(0.0f, 0.0f, 0.0f);
	cameraPos.x = _cameraDistance * std::cosf(_dispCameraPitch) * std::sinf(_dispCameraYaw);
	cameraPos.y = _cameraDistance * std::sinf(_dispCameraPitch);
	cameraPos.z = _cameraDistance * std::cosf(_dispCameraPitch) * std::cosf(_dispCameraYaw);

	cameraPos = VAdd(cameraPos, targetPos);

	_cameraPos = cameraPos;
	// 注視点を保存（描画時に使用）
	_targetPos = targetPos;
}

void Camera::Draw(int playerNum)
{
	switch (playerNum)
	{
	case 1:
		// 画面左半分をプレイヤー1用の描画範囲にする
		SetDrawArea(0, 0, WIDTH / 2, HEIGHT);
		// 消失点を左半分の中心に合わせる
		SetCameraScreenCenter(WIDTH / 4, HEIGHT / 2);
		SetupCamera_Perspective(DX_PI_F / 4.0f);
		SetCameraPositionAndTargetAndUpVec(_cameraPos, _targetPos, VGet(0.0f, 1.0f, 0.0f));
		break;
	case 2:
		// 画面右半分をプレイヤー2用の描画範囲にする
		SetDrawArea(WIDTH / 2, 0, WIDTH, HEIGHT);
		// 消失点を右半分の中心に合わせる
		SetCameraScreenCenter((WIDTH * 3) / 4, HEIGHT / 2);
		SetupCamera_Perspective(DX_PI_F / 4.0f);
		SetCameraPositionAndTargetAndUpVec(_cameraPos, _targetPos, VGet(0.0f, 1.0f, 0.0f));
		break;
	default:
		break;
	}

#ifdef _DEBUG
	DrawFormatString(3, 110, 0xffffff, "CameraMode : %d", _changedCameraMode);
#endif // DEBUG


}
