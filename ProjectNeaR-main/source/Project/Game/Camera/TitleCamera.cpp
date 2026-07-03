#include "TitleCamera.h"

namespace
{
	//カメラ距離
	constexpr float kCameraDistance = 100.0f;
	//カメラ視界高さ
	constexpr float kCameraViewPosY = 5.0f;
	//カメラ高さ
	constexpr float kCameraPosY = 100.0f;
	//ニアファー
	constexpr float kNear = 10.0f;
	constexpr float kFar = 20000.0f;

	//カメラを上下に動かす速さ
	constexpr float kCameraMoveSpeed = 0.3f;
}

TitleCamera::TitleCamera():
	CameraBase(),
	m_angle(0.0f)
{
}

TitleCamera::~TitleCamera()
{
}

void TitleCamera::Init()
{
	//値の初期化
	m_distance = kCameraDistance;
	m_vertexAngle = 0.0f;
	m_front = Vector3::Forward();
	m_right = Vector3::Right();
	m_look = m_front;
	m_rotH = Quaternion::IdentityQ();
	m_viewPos = Vector3::Zero();
	m_cameraPos = Vector3{ 0.0f,kCameraPosY,0.0f };
	m_angle = 0.0f;
}

void TitleCamera::Update()
{
	//ニアファー
	SetCameraNearFar(kNear, kFar);

	m_angle += kCameraMoveSpeed;
	if(m_angle >= 360.0f)
	{
		m_angle -= 360.0f;
	}

	m_viewPos = m_cameraPos + (m_front * m_distance);
	m_viewPos.y += kCameraViewPosY * cosf(m_angle * MyMath::DEG_2_RAD);

	//反映 
	DxLib::SetCameraPositionAndTarget_UpVecY(
		m_cameraPos.ToDxLibVector(),
		m_viewPos.ToDxLibVector()
	);
}

void TitleCamera::Draw() const
{
}
