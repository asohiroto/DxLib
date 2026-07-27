#pragma once
#include<memory>
#include <DxLib.h>

class Player;

class Camera
{
public:
	Camera();
	virtual ~Camera();
	void Init();
	void End();
	void Update(std::shared_ptr<Player> pPlayer);
	void Draw();

private:
	// 水平方向のカメラの回転角度
	float _cameraYaw;
	// 垂直方向のカメラの回転角度
	float _cameraPitch;
	// カメラの座標
	VECTOR _cameraPos;
	// 注視点
	VECTOR _targetPos;
};