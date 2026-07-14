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
	int _bgH;
	int _fadeInAlpha;
	int _startCount;
	int _ctsY1, _ctsY2;
	int _cloudH;
	int _cloudWid;
	int _scrollX;
	int _logoH;
	int _logoSH;
};
