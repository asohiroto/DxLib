#pragma once
#include <memory>

class SceneMain;
class LoadScene;

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
	// シーンを切り替え
	void ChangeScene(SceneState nextScene);
private:
	// メインシーンの共有ポインタ
	std::shared_ptr<SceneMain> p_SceneMain;
	// ロードシーンの共有ポインタ
	std::shared_ptr<LoadScene> p_LoadScene;

	// 現在のシーン
	SceneState _nowScene;
};