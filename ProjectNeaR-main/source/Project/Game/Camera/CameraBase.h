#pragma once
#include "../../General/Collision/Rigidbody.h"
#include "../../General/Math/MyMath.h"
#include <memory>
class CameraBase abstract:
	public std::enable_shared_from_this<CameraBase>
{
public:
	CameraBase();
	virtual ~CameraBase();
	virtual void Init() abstract;
	virtual void Update() abstract;
	virtual void Draw() const abstract;
	const Vector3& GetPos() const { return m_cameraPos; };
	const Vector3& GetLook() const { return m_look; };
	const Vector3& GetForward() const { return m_front; };
	const Quaternion& GetRotH() const { return m_rotH; };
	float GetVAngle()const { return m_vertexAngle; };
	//右
	const Vector3& GetRight() const { return m_right; };
	//注視点設定
	void SetViewPos(const Vector3& pos) { m_viewPos = pos; };
	//カメラ位置設定
	void SetPos(const Vector3& pos) { m_cameraPos = pos; };
	//カメラの揺れ
	void CameraShake(int frame, int shakePower);
protected:
    //カメラ距離
    float m_distance;
    //上下制限
    float m_vertexAngle;
    //向き
	Vector3 m_front;	 //前方向
	Vector3 m_right;	 //右方向
	Vector3 m_look;		 //注視方向
    //回転
    Quaternion m_rotH;
    //位置
	Vector3 m_viewPos; //注視点
	Vector3 m_cameraPos; //カメラ位置
	//カメラを揺らす
	int m_shakeFrame;
	int m_shakeCountFrame;
	int m_shakePower;

	//カメラを揺らす
	void UpdateCameraShake();

};

