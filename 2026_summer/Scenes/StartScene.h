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
	void Update(const std::shared_ptr<Input>& pInput)override;
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
	// Xボタンのハンドル
	int _xButtonH;
	// 左右の揺れを含めても外周セーフゾーンに収まるよう調整した後の文字サイズ
	int _fontSize;
	// 同上、Xボタンアイコンのサイズ
	int _iconSize;
	// 同上、文字とアイコンの間隔
	int _iconMargin;
	// 文字の縦中心とアイコンの縦中心が一致するよう調整した後のアイコンY座標
	int _iconY;
};