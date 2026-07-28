#pragma once
#include <DxLib.h>
#include <memory>

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
	virtual void Update(std::shared_ptr<Input> p_Input) {};
	virtual void Draw() {};

};

