#pragma once
#include<DxLib.h>

class GameObject
{
public:
	GameObject() {};
	virtual ~GameObject() {};
	virtual void Init() abstract;
	virtual void Update() abstract;
	virtual void Draw() const abstract;

protected:
	VECTOR _pos{ 0.0f, 0.0f, 0.0f };
	VECTOR _vec{ 0.0f, 0.0f, 0.0f };
};

