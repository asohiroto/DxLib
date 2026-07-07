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
	// 注視点
	VECTOR targetPos = VGet(0.0f, 0.0f, 0.0f);

	// プレイヤーが存在していることを確認してから、注視点をプレイヤーの座標に
	if (pPlayer != nullptr)
	{
		targetPos = pPlayer->GetPos();
	}
	
	// カメラ位置
	VECTOR cameraPos = targetPos;

	_cameraYaw += CAMERA_YAW_SPEED * std::



	SetCameraPositionAndTargetAndUpVec()
}
