#include "SE.h"
#include <DxLib.h>

namespace
{
	constexpr int kAdjustmentValue = 30;
}

SE::SE(int handle, SoundManager::SoundVolume volume,bool isLoop) :
	SoundBase(handle, volume, isLoop)
{
}

SE::~SE()
{
}
void SE::Update()
{
	SetVolume(SoundManager::GetInstance().GetSEVolume());
}

void SE::SetVolume(SoundManager::SoundVolume volume)
{
	m_volume = volume;
	ChangeVolumeSoundMem(static_cast<int>(m_volume) + kAdjustmentValue, m_playHandle);
}
