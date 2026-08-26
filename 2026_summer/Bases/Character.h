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
		bool isHit;
		// 当たり判定の色
		int color;
		// 現在体力
		int hp;
		// 最大体力
		int maxHp;
		// 現在魔力
		float mp;
		// 最大魔力
		float maxMp;
		// ジャスト回避判定の半径
		float justRadius;
		// 必殺技チャージ量
		float ultCharge;
		// 必殺技の最大チャージ量
		float maxUltCharge;
	};

public:
	Character() {};
	virtual ~Character() {};
	virtual void Init(int handle) {};
	virtual void End() {};
	virtual void Update() {};
	virtual void Draw() {};

protected:
	// ヒットボックス描画用関数
	void DrawHitBox(CharacterData data)
	{
		DrawCapsule3D(data.segmentStPos, data.segmentEndPos, data.radius, 16, data.color, data.color, false);
		DrawCapsule3D(data.segmentStPos, data.segmentEndPos, data.justRadius, 16, 0xffff00, 0xffff00, false);
	}
};

