#pragma once
#include <memory>

class SceneMain;
class LoadScene;
class StartScene;
class ClearScene;
class GameOverScene;
class Input;

class SceneManager
{
public:
	enum class SceneState
	{
		Load,
		Start,
		Main,
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
	std::shared_ptr<SceneMain> p_Main;
	// ロードシーンの共有ポインタ
	std::shared_ptr<LoadScene> p_Load;
	// インプットクラスの共有ポインタ
	std::shared_ptr<Input> p_Input;
	// スタートシーンの共有ポインタ
	std::shared_ptr<StartScene> p_Start;
	// クリアシーンの共有ポインタ
	std::shared_ptr<ClearScene> p_Clear;
	// ゲームオーバーシーンの共有ポインタ
	std::shared_ptr<GameOverScene> p_GameOver;

	// 現在のシーン
	SceneState _nowScene;
};