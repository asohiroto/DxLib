#pragma once
#include"DxLib.h"

class Camera
{
public:
	Camera();
	virtual ~Camera();
	void Init();
	// カメラの動きを管理
	// カメラが追従するために、プレイヤーの位置座標を引数とする
	void Update(VECTOR playerPos);
	// 左右のアングルを渡すゲッター
	float GetCameraAngle() const { return _cameraAngleX; }

private:
	// ジョイパッドの出力
	DINPUT_JOYSTATE _input;
	// ジョイパッドの出力の保持
	int _pad;
	// 左右のアングルの位置
	float _cameraAngleX;
	// 上下のアングルの位置
	float _cameraAngleY;
	// カメラの座標
	VECTOR _cameraPos;
	// カメラの注視点
	VECTOR _cameraTarget;
};

