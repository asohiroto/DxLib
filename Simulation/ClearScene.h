#pragma once
#include"SceneBase.h"

class ClearScene : public SceneBase
{
public:
	ClearScene(SceneManager& _sceneManager);
	~ClearScene() override;
	void Init() override;
	void End() override;
	void Update() override;
	void Draw() override;

private:
	//　矢印と、矢印の影のX座標の変化量
	int _arrow1X, _arrow2X;
	// クリアの文字と、クリアの文字の影のY座標の変化量
	int _clear1Y, _clear2Y;
};

