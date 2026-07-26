#pragma once
#include<memory>

class SceneManager;
class Input;

class TitleScene
{
public:
	TitleScene();
	~TitleScene();
	void Init(SceneManager* pManager);
	void Update(std::shared_ptr<Input> pInput);
	void Draw();

private:
	SceneManager* p_Manager;

	// タイトルのスクリーン座標
	int _titlePosX, _titlePosY;
	// 次のシーンに進むUIのスクリーン座標
	int _nextPosX, _nextPosY;

	int _logoH;
};