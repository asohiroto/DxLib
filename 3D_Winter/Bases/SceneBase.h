#pragma once

/// シーンの基底クラス
class SceneBase {
public:
    SceneBase() = default;
    virtual ~SceneBase() = default;
    virtual void Init() {}
    virtual void End() {}
    virtual void Update() {}
    virtual void Draw() {}
};
