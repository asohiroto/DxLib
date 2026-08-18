#pragma once
#include <memory>

class SceneMain;

class SceneManager
{
public:
	enum class SceneState
	{
		Load,
		Start,
		Game,
		Clear,
		GameOver
	};

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


	// 現在のシーン
	SceneState _nowScene;
};