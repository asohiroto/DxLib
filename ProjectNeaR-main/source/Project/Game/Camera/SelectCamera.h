#pragma once
#include "CameraBase.h"
#include "../../General/Math/MyMath.h"
class SelectCamera :
    public CameraBase
{
public:
	SelectCamera();
	~SelectCamera();
	void Init() override;
	void Update() override;
	void Draw() const override;

	//引きカメラ
	void PullCamera();
	//ズームカメラ
	void ZoomInCamera();
private:
	//次の視点
	Vector3 m_nextViewPos;
	//次の座標
	Vector3 m_nextPos;

	//視野角
	float m_perspective;
	float m_nextPerspective;
};

