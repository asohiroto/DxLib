#include "CameraBase.h"

CameraBase::CameraBase():
    m_distance(0.0f),
	m_vertexAngle(0.0f),
    m_front(Vector3::Forward()),
    m_right(Vector3::Right()),
    m_look(m_front),
    m_rotH(Quaternion::IdentityQ()),
	m_viewPos(Vector3::Zero()),
	m_cameraPos(Vector3::Zero()),
    m_shakeFrame(0),
    m_shakeCountFrame(0),
    m_shakePower(0)
{
}

CameraBase::~CameraBase()
{
}

void CameraBase::CameraShake(int frame, int shakePower)
{
    if (m_shakeCountFrame < frame)
    {
        m_shakeFrame = frame;
        m_shakeCountFrame = frame;
        m_shakePower = shakePower;
    }
}

void CameraBase::UpdateCameraShake()
{
    if (m_shakeCountFrame > 0)
    {
        --m_shakeCountFrame;
        //0〜1の進行率
        float t = 1.0f - static_cast<float>(m_shakeCountFrame) / m_shakeFrame;
        //減衰
        float attenuation = 1.0f - t;
        attenuation = attenuation * attenuation;
        //揺れの周期
        float wave = std::cos(t * 100.0f);
        //揺れの強さ
        float power = m_shakePower * attenuation * wave;
        //揺れる
        Vector3 shakeVec = m_front.Cross(Vector3::Up()) * power;
        Vector3 shakePos = m_cameraPos + shakeVec;
        Vector3 shakeViewPos = m_viewPos + shakeVec;
        DxLib::SetCameraPositionAndTarget_UpVecY(
            shakePos.ToDxLibVector(),
            shakeViewPos.ToDxLibVector()
        );
    }
}
