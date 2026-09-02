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
	void Update(std::shared_ptr<Input> pInput) override;
	void Draw() override;
	// シーンの遷移を行えるか
	bool CanSceneChange()const { return _canSceneChange; }

private:
	bool _canSceneChange;
	int _score;
	int _domeH;
	float _count;
};