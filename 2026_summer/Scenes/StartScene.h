#pragma once
#include "Bases/SceneBase.h"
#include <memory>

class Input;
class SkyDome;

class StartScene : public SceneBase
{
public:
	StartScene();
	virtual ~StartScene() override;
	void Init(int domeH);
	void End() override;
	void Update(std::shared_ptr<Input> pInput)override;
	void Draw()override;

private:
	// スカイドームの共有ポインタ
	std::shared_ptr<SkyDome> p_Dome;
	// ドーム回転用の角度
	float _angle;
	// タイトルロゴ
	int _logoH;
	// スタート画面のUI
	int _startY1;
	// スタート画面のUIその２
	int _startY2;
	// カウンタ
	int _count;
};