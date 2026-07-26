#pragma once
#include<memory>
#include<DxLib.h>

class SceneManager;
class Input;

class TitleScene
{
public:
	TitleScene();
	~TitleScene();
	void Init(SceneManager* pManager, int modelH, int modelSubH, int skyDomeH);
	void Update(std::shared_ptr<Input> pInput);
	void Draw();

private:
	SceneManager* p_Manager;

	// タイトルのスクリーン座標
	int _titlePosX, _titlePosY;
	// 次のシーンに進むUIのスクリーン座標
	int _nextPosX, _nextPosY;
	// もろもろのハンドル
	int _logoH;
	int _modelH;
	int _modelSubH;
	int _skyDomeH;
	// タイトルシーンのアニメーションの番号
	int _titleAnimIndex;
	// アニメーション再生用のカウンタ
	float _animCount;
	// ２体目のアニメーションの番号
	int _subAnimIndex;
	// ２体目のアニメーション再生用のカウンタ
	float _subCount;

	// モデルの座標
	VECTOR _modelPos;
	VECTOR _subPos;
};