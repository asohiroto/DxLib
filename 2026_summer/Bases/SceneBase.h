#pragma once
#include <memory>

class Input;

class SceneBase
{
public:
	SceneBase() {};
	virtual ~SceneBase() {};
	virtual void Init() {};
	virtual void End() {};
	virtual void Update(std::shared_ptr<Input> pInput) {};
	virtual void Draw() {};
	// シーン遷移を行うか
	bool CanSceneChange() const { return _isSceneChange; }
protected:
	// シーン遷移を行うか
	bool _isSceneChange = false;
};