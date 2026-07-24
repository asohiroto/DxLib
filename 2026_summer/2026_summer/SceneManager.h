#pragma once
#include <memory>

class SceneMain;

class SceneManager
{
public:
	SceneManager();
	virtual ~SceneManager();
	void Init();
	void End();
	void Update();
	void Draw();

private:
	// メインシーンの共有ポインタ
	std::shared_ptr<SceneMain> p_SceneMain;
};