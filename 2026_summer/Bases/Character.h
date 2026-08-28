#pragma once
#include "Anims/AnimInfo.h"
#include <DxLib.h>

class Character
{
public:
	enum class CharacterState
	{
		// 近づく
		Approach,
		// 遠ざかる
		MoveAway,
		// 左移動
		MoveLeft,
		// 右移動
		MoveRight,
		// 右側回避
		DodgeRight,
		// 左側回避
		DodgeLeft,
		// マジックショット
		Shot,
		// マジックミサイル
		Missile,
		// マジックフューリー
		Fury,
		// マジックビーム
		Beam,
		// 被弾硬直
		HitStun,
		// 死亡
		Dead
	};

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
		// 現在のステート
		CharacterState nowState;
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

	// ステートに応じて数値を取得する
	static AnimInfo TranslateState(CharacterState state)
	{
		switch (state)
		{
		case CharacterState::Approach:
			return { 7, true, 60 };
		case CharacterState::MoveAway:
			return { 6, true, 60 };
		case CharacterState::MoveLeft:
			return { 8, true, 60 };
		case CharacterState::MoveRight:
			return { 9, true, 60 };
		case CharacterState::Shot:
			return { 5, false, 20 };
		case CharacterState::Missile:
			return { 4, false, 30 };
		case CharacterState::Fury:
			return { 3, false, 30 };
		case CharacterState::Beam:
			return { 4, false, 30 };
		case CharacterState::DodgeLeft:
			return { 0, false, 15 };
		case CharacterState::DodgeRight:
			return { 1, false, 15 };
		case CharacterState::Dead:
			return { 11, false, 30 };
		case CharacterState::HitStun:
			return { 10, false, 5 };
		default:
			return { 7, true, 10 };
		}
	}
};