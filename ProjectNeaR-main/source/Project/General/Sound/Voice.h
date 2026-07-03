#pragma once
#include "SoundBase.h"
#include "SoundManager.h"

class Voice :
    public SoundBase
{
public:
    Voice(int handle, SoundManager::SoundVolume volume);
    ~Voice();
    void Update()override;

    //‰¹—Ê
    void SetVolume(SoundManager::SoundVolume volume) override;
};

