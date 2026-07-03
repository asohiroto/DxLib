#pragma once
#include "SoundBase.h"
#include "SoundManager.h"
class BGM :
    public SoundBase
{
public:
	BGM(int handle, SoundManager::SoundVolume volume);
	~BGM();
	void Update()override;
	void ChangeBGM(int handle);
};

