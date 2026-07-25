#pragma once
#include <memory>

class SceneManager;
class Input;

class ResultScene
{
public:
	ResultScene();
	~ResultScene();
	void Init(SceneManager* pManager, int winner);
	void Update(std::shared_ptr<Input> pInput);
	void Draw();

private:
	int _winner;

	SceneManager* p_Manager;
};

