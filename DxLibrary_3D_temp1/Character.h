#pragma once
#include<DxLib.h>

class Character
{
public:
	Character() {};
	virtual ~Character() {};
	virtual void Init() {};
	virtual void Update() {};
	virtual void Draw() {};
	// _posのゲッター
	VECTOR GetPos() const { return _pos; }
	void SetPos(VECTOR pos) { _pos = pos; }
protected:
	// モデルのハンドル
	int _modelH = -1;
	// 位置
	VECTOR _pos = VGet(0.0f, 0.0f, 0.0f);
};

