#include "Timer.h"
#include "../Main/Application.h"

namespace
{
	constexpr float kMaxTime = 9999999.0f;
	constexpr float kMillisecond = 60.0f;
	constexpr float kSeconds = 60.0f;
	constexpr float kMinutes = 60.0f;
}

Timer::Timer()
{
	Init();
}

Timer::~Timer()
{
}

void Timer::Init()
{
	m_time = 0.0f;
	m_millisecond = 0.0f;
	m_seconds = 0;
	m_minutes = 0;
	m_isUpdate = true;
}

void Timer::Update()
{
	if (!m_isUpdate)return;
	float timeScale = Application::GetInstance().GetTimeScale();
	if (m_time < kMaxTime)
	{
		//‘ƒtƒŒ[ƒ€”
		m_time += timeScale;
		//ƒ~ƒŠ•b‚ðXV
		m_millisecond += timeScale;
		if (m_millisecond >= kMillisecond)
		{
			m_millisecond = 0;
			++m_seconds;

			//•b‚ª60‚ð’´‚¦‚½‚ç•ª‚ÉŒJ‚èã‚°
			if (m_seconds >= kSeconds)
			{
				m_seconds = 0;
				++m_minutes;
			}
		}
	}
}
