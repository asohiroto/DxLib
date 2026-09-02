#pragma once
#include "Bases/SceneBase.h"

class Input;

class ExplainScene : public SceneBase
{
public:
	ExplainScene();
	virtual ~ExplainScene() override;
	void Init() override;
	void End() override;
	void Update(std::shared_ptr<Input> pInput) override;
	void Draw() override;
	// シーンの遷移を行うか
	bool CanSceneChange() const { return _canSceneChange; }
private:
	// 紹介画像ハンドル
	int _explainH;
	// シーンの遷移を行うか
	bool _canSceneChange;
};