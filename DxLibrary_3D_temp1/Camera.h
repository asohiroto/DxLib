#pragma once
#include<memory>

class Player;
class Input;

class Camera
{
public:
	Camera();
	virtual ~Camera();
	void Init();
	void Update(std::shared_ptr<Player> pPlayer);

private:
	// カメラとの距離
	float _cameraDistance;
	// 水平方向の角度
	float _cameraYaw;
	// 垂直方向の角度
	float _cameraPitch;
	// インプットクラスのポインタ
	std::shared_ptr<Input> p_Input;
};

