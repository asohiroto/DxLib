#pragma once
#include"SceneBase.h"

class ClearScene : public SceneBase
{
public:
	ClearScene(SceneManager& _sceneManager);
	~ClearScene() override;
	void Init() override;
	void End() override;
	void Update() override;
	void Draw() override;

private:
	int _mousePosX, _mousePosY;
	int _bgH;
};

