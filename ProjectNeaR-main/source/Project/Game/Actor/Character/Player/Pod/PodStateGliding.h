#pragma once
#include "PodStateBase.h"
class Actor;
class PodStateGliding :
    public PodStateBase, public std::enable_shared_from_this<PodStateGliding>
{
public:
    PodStateGliding(std::weak_ptr<Actor>  pod);
    ~PodStateGliding();
    void Init()override;
    void Update() override;
private:
};

