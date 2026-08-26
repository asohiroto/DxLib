#pragma once
#include "Bases/SceneBase.h"
#include <memory>

class Input;

class GameOverScene : public SceneBase
{
public:
	GameOverScene();
	virtual ~GameOverScene() override;
	void Init() override;
	void End() override;
	void Update(std::shared_ptr<Input> pInput) override;
	void Draw() override;
};