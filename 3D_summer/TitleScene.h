#pragma once
#include<memory>

class SceneManager;

class TitleScene
{
public:
	TitleScene();
	~TitleScene();
	void Init(SceneManager* pManager);
	void Update();
	void Draw();

private:
	SceneManager* p_Manager;
};