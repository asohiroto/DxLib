#pragma once
#include <memory>

class SceneMain;

/// シーンを管理
class SceneManager
{
public:
	enum class SceneType
	{
		Main
	};
public:
	SceneManager();
	virtual ~SceneManager();
	void Init();
	void End();
	void Update();
	void Draw();

private:
	std::unique_ptr<SceneMain> p_Main;
	SceneType _nowScene;
};
