#pragma once

/// キャラクターの基底クラス
class Character 
{
    public:
    struct CharacterInfo
    {
        
    };

public:
    Character() = default;
    virtual ~Character() = default;
    void Init() {}
    void End() {}
    void Update() {}
    void Draw() {}
};
