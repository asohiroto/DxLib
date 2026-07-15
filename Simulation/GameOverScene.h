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

private:
	int _over1Y, _over2Y;
};

