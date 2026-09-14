#pragma once

#include "../Bases/SceneBase.h"

/// ゲームのメインとなるシーン
class SceneMain : public SceneBase {
public:
    SceneMain();
    virtual ~SceneMain() override;
    void Init() override;
    void End() override;
    void Update() override;
    void Draw() override;
};
