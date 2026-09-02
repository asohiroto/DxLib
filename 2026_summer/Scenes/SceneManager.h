#pragma once
#include <memory>

class SceneMain;
class LoadScene;
class StartScene;
class ResultScene;
class ExplainScene;
class Input;

class SceneManager
{
public:
	enum class SceneState
	{
		Load,
		Explain,
		Start,
		Main,
		Result
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
	// フェードインを行う関数
	void Fade(int count);

private:
	// メインシーンの共有ポインタ
	std::shared_ptr<SceneMain> p_Main;
	// ロードシーンの共有ポインタ
	std::shared_ptr<LoadScene> p_Load;
	// インプットクラスの共有ポインタ
	std::shared_ptr<Input> p_Input;
	// スタートシーンの共有ポインタ
	std::shared_ptr<StartScene> p_Start;
	// リザルトシーンの共有ポインタ
	std::shared_ptr<ResultScene> p_Result;

	std::shared_ptr<ExplainScene> p_Explain;

	// 現在のシーン
	SceneState _nowScene;

	// スコア
	int _score;
	// 倒した数
	int _defeatNum;
	// カウンタ
	int _count;
};