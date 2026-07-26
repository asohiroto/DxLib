#pragma once
#include<DxLib.h>
#include <memory>

class SceneManager;
class Input;

class ResultScene
{
public:
	ResultScene();
	~ResultScene();
	void Init(SceneManager* pManager, int winner, int modelH, int skyDomeH);
	void Update(std::shared_ptr<Input> pInput);
	void Draw();

private:
	int _winner;

	SceneManager* p_Manager;

	int _modelH, _skyDomeH;

	VECTOR _modelPos;

	int _attachAnimIndex;;

	float _animCount;
};

