#pragma once

class SceneManager;

class SceneBase
{
protected:
	// 引数のシーンマネージャーを常に共通のものにするために、参照
	SceneManager& _sceneManager;

public:
	SceneBase(SceneManager& _sceneManager);
	virtual ~SceneBase() = default;
	virtual void Init() {};
	virtual void End() {};
	virtual void Update() {};
	virtual void Draw() {};

protected:
	// マウスの座標
	int _mousePosX, _mousePosY;
	// 背景画像のハンドル
	int _bgH;
	// フレームのカウンタ
	int _count;
};