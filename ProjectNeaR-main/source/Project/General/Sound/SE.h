#pragma once
#include "SoundBase.h"
#include "SoundManager.h"

class SE :
    public SoundBase
{
public:
    SE(int handle, SoundManager::SoundVolume volume, bool isLoop);
    ~SE();
    void Update()override;

    //‰¹—Ê
    void SetVolume(SoundManager::SoundVolume volume) override;
};

