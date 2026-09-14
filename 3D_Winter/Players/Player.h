#pragma once

/// プレイヤーの管理クラス
class Player {
public:
    Player();
    virtual ~Player();
    void Init();
    void End();
    void Update();
    void Draw();
};
