#include "NormalEffect.h"
#include <EffekseerForDXLib.h>
#include "../../Main/Application.h"
namespace
{
	constexpr float kDefaultScale = 100.0f;
}

NormalEffect::NormalEffect(int playHandle, Vector3 pos):
	m_playHandle(playHandle),
	m_pos(pos),
	m_rot{},
	m_scale{ kDefaultScale,kDefaultScale,kDefaultScale },
	m_playSpeed(1.0f),
	m_isDelete(false),
	m_specificFrame(0.0f),
	m_isSpecificFrame(false),
	m_dir(Vector3::Back()),
	m_myTimeScale(0.0f),
	m_isMyTimeScale(false),
	m_isDraw(true)
{
	SetPos(m_pos);
	SetRot(m_rot);
	SetScale(m_scale);
}

NormalEffect::~NormalEffect()
{
	StopEffekseer3DEffect(m_playHandle);
}

void NormalEffect::Update()
{
	if (!m_isDraw)return;

	//自分のタイムスケールを使うか
	if (!m_isMyTimeScale)
	{
		//使わない場合
		m_myTimeScale = Application::GetInstance().GetTimeScale();
	}

	//再生
	Play();

	//指定フレーム再生なら
	if (m_isSpecificFrame)
	{
		if (m_specificFrame <= 0)
		{
			Delete();
			return;
		}

		//フレーム
		m_specificFrame -= m_myTimeScale;
	}
	else
	{
		//再生終了したら
		if (IsEffekseer3DEffectPlaying(m_playHandle) != 0)
		{
			Delete();
			return;
		}
	}
}

void NormalEffect::End()
{
	Delete();
}

void NormalEffect::Play()
{
	SetSpeedPlayingEffekseer3DEffect(m_playHandle, m_playSpeed * m_myTimeScale);
}

void NormalEffect::Stop()
{
	SetSpeedPlayingEffekseer3DEffect(m_playHandle, 0.0f);
}

void NormalEffect::SetPlaySpeed(float speed)
{
	m_playSpeed = speed;
	SetSpeedPlayingEffekseer3DEffect(m_playHandle, m_playSpeed);
}

void NormalEffect::Delete()
{
	StopEffekseer3DEffect(m_playHandle);
	m_isDelete = true;
}

void NormalEffect::SetPos(Vector3 pos)
{
	m_pos = pos;
	SetPosPlayingEffekseer3DEffect(m_playHandle,
		m_pos.x, m_pos.y, m_pos.z);
}

void NormalEffect::SetRot(Vector3 rot)
{
	m_rot = rot;
	//おそらく相対ではなく絶対
	SetRotationPlayingEffekseer3DEffect(m_playHandle,
		m_rot.x, m_rot.y, m_rot.z);
}

void NormalEffect::SetScale(Vector3 scale)
{
	m_scale = scale;
	SetScalePlayingEffekseer3DEffect(m_playHandle,
		m_scale.x, m_scale.y, m_scale.z);
}

void NormalEffect::LookAt(Vector3 dir)
{
	auto nextDir = dir;
	//正規化
	if (nextDir.SqMagnitude() > 0.0f)
	{
		nextDir = nextDir.Normalize();
	}
	//Y角度
	float yRad = Vector2::GetRad(m_dir.XZ(), nextDir.XZ());
	//外積から回転方向を計算
	float cross = m_dir.XZ().Cross(nextDir.XZ());
	if (cross > 0)
	{
		yRad *= -1;
	}
	//角度をセット
	Vector3 rot = { 0.0f,yRad,0.0f};
	SetRot(rot);
}

void NormalEffect::SpecificFrame(float frame)
{
	m_specificFrame = frame;
	m_isSpecificFrame = true;
}

void NormalEffect::SetTimeScale(float scale)
{
	m_myTimeScale = scale;
	EnableIsMyScale();
}

void NormalEffect::EnableIsMyScale()
{
	m_isMyTimeScale = true;
}

void NormalEffect::DisableIsMyScale()
{
	m_isMyTimeScale = false;
}