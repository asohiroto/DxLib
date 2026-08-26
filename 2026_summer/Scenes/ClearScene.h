#pragma once
#include "Bases/SceneBase.h"
#include <memory>

class Input;

class ClearScene : public SceneBase
{
public:
	ClearScene();
	virtual ~ClearScene() override;
	void Init() override;
	void End() override;
	void Update(std::shared_ptr<Input> pInput) override;
	void Draw() override;
};