#pragma once
#include <memory>
#include "SoundManager.h"
class SoundBase abstract:
	public std::enable_shared_from_this<SoundBase>
{
public:
	SoundBase(int handle, SoundManager::SoundVolume volume,bool isLoop);
	~SoundBase();
	virtual void Init();
	virtual void Update()abstract;
	virtual void End();
	//再生
	virtual void Play();
	//停止
	virtual void Stop();
	//削除
	virtual void Delete();
	//再生中ならtrue
	virtual bool IsPlay();
	//削除予定
	virtual bool IsDelete();
	//音量
	virtual void SetVolume(SoundManager::SoundVolume volume);
protected:
	//ハンドル
	int m_playHandle;
	//音量
	SoundManager::SoundVolume m_volume;
	//ループ
	bool m_isLoop;
};

