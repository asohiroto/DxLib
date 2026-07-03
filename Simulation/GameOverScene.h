#pragma once
#include "SceneBase.h"

class GameOverScene : public SceneBase
{
public:
	GameOverScene();
	~GameOverScene()override;
	void Init() override;
	void End() override;
	void Update() override;
	void Draw() override;
};

