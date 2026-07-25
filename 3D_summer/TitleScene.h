#pragma once
#include<memory>

class SceneManager;

class TitleScene
{
public:
	TitleScene();
	~TitleScene();
	void Init();
	void Update();
	void Draw();

private:
	std::shared_ptr<SceneManager> p_Manager;
};