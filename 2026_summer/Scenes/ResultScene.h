#pragma once
#include "Bases/SceneBase.h"
#include <memory>

class Input;

class ResultScene : public SceneBase
{
public:
	ResultScene();
	virtual ~ResultScene() override;
	void Init(int score, int handle);
	void End() override;
	void Update(const std::shared_ptr<Input>& pInput) override;
	void Draw() override;
	// シーンの遷移を行えるか
	bool CanSceneChange()const { return _canSceneChange; }

private:
	// シーンの遷移を行うか
	bool _canSceneChange;
	// スコア
	int _score;
	// スカイドームのモデルハンドル
	int _domeH;
	// ドーム回転用の角度
	float _count;
	// UI点滅表示用のカウンタ
	int _uiCount;
};