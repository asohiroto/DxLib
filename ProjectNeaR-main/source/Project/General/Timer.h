#pragma once
#include <memory>
class Timer :
	public std::enable_shared_from_this<Timer>
{
public:
	Timer();
	~Timer();
	void Init();
	void Update();
	float GetTime() { return m_time; };
	int GetMillisecond() { return static_cast<int>(m_millisecond); };
	int GetSeconds() { return m_seconds; };
	int GetMinutes() { return m_minutes; };
	void StopUpdate() { m_isUpdate = false; };
	void StartUpdate() { m_isUpdate = true; };
private:
	float m_time;
	float m_millisecond;
	int m_seconds;
	int m_minutes;
	//XV‚ğ‚·‚é‚©
	bool m_isUpdate;
};

