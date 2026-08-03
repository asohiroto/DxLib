#pragma once
#include<memory>
#include <DxLib.h>

class Player;
class Input;
class Enemy;

class Camera
{
public:
	Camera();
	virtual ~Camera();
	void Init();
	void End();
	void Update(std::shared_ptr<Player> pPlayer, std::shared_ptr<Enemy>pEnemy, std::shared_ptr<Input> pInput);
	void Draw();
	// カメラの水平方向の角度のゲッター
	float GetCameraYaw() const { return _cameraYaw; }
	// 通常のカメラ
	void NormalCam(int rx, int ry, std::shared_ptr<Player> pPlayer);
	// ロックオン時のカメラ
	void LockOnCam(int rx, int ry, std::shared_ptr<Player>pPlayer, std::shared_ptr<Enemy>pEnemy);

private:
	// 水平方向のカメラの回転角度
	float _cameraYaw;
	// 垂直方向のカメラの回転角度
	float _cameraPitch;
	// 表示用のカメラの水平方向回転角度
	float _dispCameraYaw;
	// 表示用のカメラの垂直方向回転角度
	float _dispCameraPitch;
	// カメラの座標
	VECTOR _cameraPos;
	// 注視点
	VECTOR _targetPos;
	// カメラモード
	// trueでロックオンモード
	// falseで通常カメラモード
	bool _cameraMode;
};