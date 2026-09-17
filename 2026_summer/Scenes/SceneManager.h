#pragma once
#include "Sounds/SoundInfo.h"
#include "SceneMain.h"
#include "LoadScene.h"
#include "StartScene.h"
#include "ResultScene.h"
#include "ExplainScene.h"
#include "Inputs/Input.h"

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

private:
	// シーンを切り替え
	void ChangeScene(SceneState nextScene);
	// フェードインを行う関数
	void Fade(int count);

private:
	// メインシーン
	SceneMain _Main;
	// ロードシーン
	LoadScene _Load;
	// インプットクラス
	Input _Input;
	// スタートシーン
	StartScene _Start;
	// リザルトシーン
	ResultScene _Result;
	// 説明シーン
	ExplainScene _Explain;

	// 現在のシーン
	SceneState _nowScene;

	// スコア
	int _score;
	// 倒した数
	int _defeatNum;
	// カウンタ
	int _count;
	// BGM
	BgmHandles _gameBgm;
	// SE
	SeHandles _gameSe;
	// プレイヤーが死んでからのカウンタ
	int _deadCount;
};