#include "SelectCamera.h"

namespace
{
    //ニアファー
    constexpr float kNear = 10.0f;
    constexpr float kFar = 20000.0f;

    //距離
    constexpr float kDistance = -100.0f;
    //Lerp
    constexpr float kViewLerp = 0.1f;
    constexpr float kPosLerp = 0.1f;
    constexpr float kPerLerp = 0.1f;


    //カメラの視野角
    constexpr float kPullPerspective = 60.0f * MyMath::DEG_2_RAD;
    constexpr float kZoomInPerspective = 50.0f * MyMath::DEG_2_RAD;

    //初期位置
    const Vector3 kFirstPos = { 0.0f,20.0f,0.0f };
}

SelectCamera::SelectCamera():
	CameraBase(),
    m_nextViewPos(),
    m_nextPos(),
    m_perspective(kPullPerspective),
    m_nextPerspective(kPullPerspective)
{
}

SelectCamera::~SelectCamera()
{
}

void SelectCamera::Init()
{
    m_distance = kDistance;
    m_vertexAngle = 0.0f;
    m_front = Vector3::Forward();
    m_right = Vector3::Right();
    m_look = m_front;
    m_rotH = Quaternion::IdentityQ();
    m_cameraPos = kFirstPos;
    m_viewPos = m_cameraPos + (m_front * m_distance);
    m_nextViewPos = m_viewPos;
    m_nextPos = m_cameraPos;
    m_perspective = kPullPerspective;
    m_nextPerspective = m_perspective;
}

void SelectCamera::Update()
{
    SetCameraNearFar(kNear, kFar);

    //視点
    m_nextViewPos = Vector3::Lerp(m_nextViewPos, m_viewPos, kViewLerp);

    //座標
    m_nextPos = Vector3::Lerp(m_nextPos, m_cameraPos, kPosLerp);

    //視野角
    m_perspective = MathSub::Lerp(m_perspective, m_nextPerspective, kPerLerp);

    // DxLibに反映
    DxLib::SetCameraPositionAndTarget_UpVecY(
        m_nextPos.ToDxLibVector(),
        m_nextViewPos.ToDxLibVector()
    );
    //視野角
    SetupCamera_Perspective(m_perspective);
}

void SelectCamera::Draw() const
{
}

void SelectCamera::PullCamera()
{
    m_nextPerspective = kPullPerspective;
}

void SelectCamera::ZoomInCamera()
{
    m_nextPerspective = kZoomInPerspective;
}
