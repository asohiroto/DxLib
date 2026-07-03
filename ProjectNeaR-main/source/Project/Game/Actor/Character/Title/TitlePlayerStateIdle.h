#pragma once
#include "../CharacterStateBase.h"
class TitlePlayerStateIdle :
    public CharacterStateBase, public std::enable_shared_from_this<TitlePlayerStateIdle>
{
public:
    TitlePlayerStateIdle(std::weak_ptr<Actor>  player);
    ~TitlePlayerStateIdle();
    void Init()override;
    void Update() override;
private:
};
