#pragma once
#include "PodStateBase.h"
class Actor;
class PodStateIdle :
    public PodStateBase, public std::enable_shared_from_this<PodStateIdle>
{
public:
    PodStateIdle(std::weak_ptr<Actor>  pod);
    ~PodStateIdle();
    void Init()override;
    void Update() override;
private:
    //è„â∫Ç…óhÇÍÇÈ
    float m_swayAngle;
};

