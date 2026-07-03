#include "SoundBase.h"
#include "../LoadingManager.h"
#include <DxLib.h>

SoundBase::SoundBase(int handle, SoundManager::SoundVolume volume, bool isLoop):
	m_playHandle(handle),
	m_volume(volume),
	m_isLoop(isLoop)
{
	SetVolume(m_volume);
}

SoundBase::~SoundBase()
{
	Delete();
}

void SoundBase::Init()
{
	SoundManager::GetInstance().Entry(shared_from_this());
}
void SoundBase::End()
{
	Delete();
	SoundManager::GetInstance().Exit(shared_from_this());
}
void SoundBase::Play()
{
	//ロード中は停止
	if (LoadingManager::GetInstance().IsLoading())return Stop();

	//非同期読み込みが終了したか
	if(CheckHandleASyncLoad(m_playHandle))return;
	if(m_playHandle == -1)return;

	SetVolume(m_volume);
	//再生
	if (m_isLoop)	
	{
		//ループ
		PlaySoundMem(m_playHandle, DX_PLAYTYPE_LOOP, true);
	}
	else
	{
		PlaySoundMem(m_playHandle, DX_PLAYTYPE_BACK, true);
	}
}
void SoundBase::Stop()
{
	if (m_playHandle == -1)return;
	//再生を止める
	StopSoundMem(m_playHandle);
}
void SoundBase::Delete()
{
	if (m_playHandle == -1)return;
	//削除
	DeleteSoundMem(m_playHandle);
	m_playHandle = -1;
}

bool SoundBase::IsPlay()
{
	return CheckSoundMem(m_playHandle);
}

bool SoundBase::IsDelete()
{
	//ループ再生じゃなく再生が終了したとき削除する
	return !IsPlay();
}

void SoundBase::SetVolume(SoundManager::SoundVolume volume)
{
	m_volume = volume;
	ChangeVolumeSoundMem(static_cast<int>(m_volume), m_playHandle);
}
