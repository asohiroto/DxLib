#pragma once
#include "Vec3.h"

/// キャラクターの基底クラス
class Character 
{
public:
    struct CharacterInfo
    {
        Vec3 pos;
        int handle;
    };

public:
    Character() = default;
    virtual ~Character() = default;
    void Init() {}
    void End() {}
    void Update() {}
    void Draw() {}
};
