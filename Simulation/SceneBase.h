#pragma once
#include"SceneManager.h";
#include"SceneName.h"

class SceneBase
{
public:
	SceneBase() = default;
	virtual ~SceneBase() = default;
	virtual void Init() {};
	virtual void End() {};
	virtual void Update() {};
	virtual void Draw() {};

protected:
	SceneManager* p_SceneManager = new SceneManager;
	// 循環しないように、ここで処理（中身はSceneManager）
	// シーン遷移用関数
	void ChangeScene(SceneManager* sm, SceneName scene) { sm->ChangeScene(scene); }

};