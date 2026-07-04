#pragma once
#include "SceneBase.h"

class GameOverScene : public SceneBase
{
public:
	GameOverScene(SceneManager& _sceneManager);
	~GameOverScene()override;
	void Init() override;
	void End() override;
	void Update() override;
	void Draw() override;
};

