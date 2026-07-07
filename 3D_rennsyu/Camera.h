#pragma once
#include<cmath>
#include<DxLib.h>
#include<memory>

class Player;

class Camera
{
private:
	Camera() = default;
	~Camera() = default;

	Camera(const Camera&) = delete;
	Camera& operator = (const Camera&) = delete;

	Camera(const Camera&&) = delete;
	Camera& operator = (Camera&&) = delete;

private:
	float _cameraDistance = -1000.0f;
	// 水平方向の回転
	float _cameraYaw = 0.0f;
	// 垂直方向の回転
	float _cameraPitch = -0.5f;

public:
	static Camera& GetInstance()
	{
		static Camera instance;
		return instance;
	}

	void Init();
	void Update(std::shared_ptr<Player> player);
	// 水平方向の回転
	float GetCameraYaw() const { return _cameraYaw; }
	// 垂直方向の回転
	float GetCameraPitch() const { return _cameraPitch; }
};

