#pragma once
#include "Bases/SceneBase.h"
#include <memory>

class Input;

class StartScene : public SceneBase
{
public:
	StartScene();
	virtual ~StartScene() override;
	void Init() override;
	void End() override;
	void Update(std::shared_ptr<Input> pInput)override;
	void Draw()override;
};