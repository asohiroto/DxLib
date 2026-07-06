#pragma once
#include"DxLib.h"

class Object
{
public:
	Object();
	virtual ~Object();
	void Init();
	void Draw();

private:
	// モデルのハンドル　複数表示用
	int _handle[4];
	// モデルの座標　複数個所に配置
	VECTOR _pos[4];
};

