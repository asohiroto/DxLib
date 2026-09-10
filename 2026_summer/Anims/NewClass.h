#pragma once

#include "../Bases/SceneBase.h"

#include <memory>

class PlayerMpBar;
class SceneMain;

class NewClass : public SceneBase {
public:
    NewClass();
    virtual ~NewClass();
    void Init() override;
    void Shutdown();
    void Update(const std::shared_ptr<Input>& pInput) override;
    void Draw() override;

private:
    std::unique_ptr<PlayerMpBar> playerMpBar_;
    SceneMain* sceneMain_ = nullptr;
};
