#pragma once
#include <DxLib.h>
#include <memory>

class Character
{
public:
	struct CharacterData
	{
		// モデルのハンドル
		int modelH;
		// 座標
		VECTOR pos;
		// 【当たり判定用】線分の始点
		VECTOR segmentStPos;
		// 【当たり判定用】線分の終点
		VECTOR segmentEndPos;
		// 【当たり判定用】半径
		float radius;
		// なにかに当たっているか
		bool  isHit;
	};

public:
	Character() {};
	virtual ~Character() {};
	virtual void Init() {};
	virtual void End() {};
	virtual void Update() {};
	virtual void Draw() {};

};

