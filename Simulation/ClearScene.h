#pragma once
#include"SceneBase.h"
class ClearScene : public SceneBase
{
public:
	ClearScene();
	~ClearScene() override;
	void Init() override;
	void End() override;
	void Update() override;
	void Draw() override;

};

