#include "BGM.h"
#include <DxLib.h>


BGM::BGM(int handle, SoundManager::SoundVolume volume):
	SoundBase(handle,volume,true)
{
}

BGM::~BGM()
{
}
void BGM::Update()
{
	SetVolume(SoundManager::GetInstance().GetBGMVolume());
}

void BGM::ChangeBGM(int handle)
{
	if (m_playHandle == handle)return Play();
	//再生中のハンドルがあるなら
	if (m_playHandle >= 0)
	{
		//再生を止める
		Stop();
		//削除
		DeleteSoundMem(m_playHandle);
	}

	//ハンドル
	m_playHandle = handle;

	//再生
	Play();
}
