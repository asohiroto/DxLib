#include "Camera.h"
#include<cmath>

Camera::Camera() :
	_input(),
	_pad(0),
	_cameraAngleX(0.0f), _cameraAngleY(0.0f),
	_cameraPos(VGet(0.0f, 0.0f, 0.0f)),
	_cameraTarget(VGet(0.0f, 0.0f, 0.0f))
{

}

Camera::~Camera()
{

}

void Camera::Init()
{
	// 奥行10～8000までをカメラの描画範囲とする
	SetCameraNearFar(10.0f, 8000.0f);
	// カメラ位置の初期位置
	_cameraPos = VGet(0.0f, 0.0f, 0.0f);
	// 注視点の初期位置
	_cameraTarget = VGet(0.0f, 100.0f, 0.0f);
}

void Camera::Update(VECTOR playerPos)
{
	_pad = GetJoypadDirectInputState(DX_INPUT_PAD1, &_input);

	if (_input.Rx > 0)
	{
		_cameraAngleX += 0.05f;
	}

	if (_input.Rx < 0)
	{
		_cameraAngleX -= 0.05f;
	}

	_cameraPos = VAdd(playerPos, VTransform({ 0.0f,200.0f,-400.0f }, MGetRotY(_cameraAngleX)));
	_cameraTarget = VAdd(playerPos, { 0.0f, 100.0f, 0.0f });
	SetCameraPositionAndTarget_UpVecY(_cameraPos, _cameraTarget);
}