#pragma once
#pragma once
#include "Character.h"
#include<DxLib.h>

class Player :
    public Character
{
public:
    Player();
    ~Player() override;
    void Init() override;
    void Update() override;
    void Draw() override;

private:
    // 動く方向
    VECTOR _move;
};

