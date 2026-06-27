#pragma once

class SceneBase
{
public:
	SceneBase() = default;
	virtual ~SceneBase() = default;
	virtual void Init() {};
	virtual void Update() {};
	virtual void Draw() {};
};