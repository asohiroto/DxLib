#pragma once
#include <DxLib.h>

class Input;

class Character
{
public:
	struct CharacterData
	{
		// モデルのハンドル
		int modelH;
		// 座標
		VECTOR pos;
	};

public:
	Character() {};
	virtual ~Character() {};
	virtual void Init() {};
	virtual void End() {};
	virtual void Update() {};
	virtual void Draw() {};

};

