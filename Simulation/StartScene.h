#pragma once
#include"SceneBase.h"

class SceneManager;

class StartScene : public SceneBase
{
public:
	StartScene(SceneManager& _sceneManager);
	~StartScene() override;
	void Init() override;
	void End() override;
	void Update() override;
	void Draw() override;

private:
	int _mousePosX, _mousePosY;
};
