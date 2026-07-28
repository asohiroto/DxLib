#pragma once

class SceneBase
{
public:
	SceneBase() {};
	virtual ~SceneBase() {};
	virtual void Init() {};
	virtual void End() {};
	virtual void Update() {};
	virtual void Draw() {};
};