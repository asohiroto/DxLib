#pragma once
#include"SceneBase.h"

class StartScene : public SceneBase
{
public:
	StartScene();
	~StartScene() override;
	void Init() override;
	void Update() override;
	void Draw() override;
};
