#pragma once
#include<memory>
#include<DxLib.h>

class Player;
class Input;

class Camera
{
public:
	Camera();
	virtual ~Camera();
	void Init();
	void Update(std::shared_ptr<Player> pPlayer, std::shared_ptr<Input> pInput);
	void Draw(int playerNum);
	// カメラの水平方向の角度を渡す
	float GetCameraYaw() const { return _cameraYaw; }
private:
	// 水平方向の角度
	float _cameraYaw;
	// 垂直方向の角度
	float _cameraPitch;
	// カメラ位置の保存用（描画用）
	VECTOR _cameraPos;
	// 注視点の保存用（描画用）
	VECTOR _targetPos;
	// 線形補完後の向き
	VECTOR _smoothedForward;
};

