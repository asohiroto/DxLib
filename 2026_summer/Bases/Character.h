#pragma once
#include "Anims/AnimInfo.h"
#include <DxLib.h>

namespace
{
	// 各ステートに対応するアニメーション番号
	constexpr int ANIM_DODGE_LEFT = 0;
	constexpr int ANIM_DODGE_RIGHT = 1;
	constexpr int ANIM_WAIT = 2;
	constexpr int ANIM_FURY = 3;
	constexpr int ANIM_MISSILE = 4;
	constexpr int ANIM_BEAM = 4;
	constexpr int ANIM_SHOT = 5;
	constexpr int ANIM_MOVE_AWAY = 6;
	constexpr int ANIM_APPROACH = 7;
	constexpr int ANIM_MOVE_LEFT = 8;
	constexpr int ANIM_MOVE_RIGHT = 9;
	constexpr int ANIM_HIT_STUN = 10;
	constexpr int ANIM_DEAD = 11;

	// 各ステートのアニメーション再生フレーム数
	constexpr int MOVE_ANIM_FRAME = 60;
	constexpr int SHOT_ANIM_FRAME = 20;
	constexpr int MISSILE_ANIM_FRAME = 30;
	constexpr int FURY_ANIM_FRAME = 30;
	constexpr int BEAM_ANIM_FRAME = 30;
	constexpr int DODGE_ANIM_FRAME = 30;
	constexpr int DEAD_ANIM_FRAME = 30;
	constexpr int HIT_STUN_ANIM_FRAME = 5;
	constexpr int WAIT_ANIM_FRAME = 30;
}

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
		Dead,
		// 待機
		Wait
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
			return { ANIM_APPROACH, true, MOVE_ANIM_FRAME };
		case CharacterState::MoveAway:
			return { ANIM_MOVE_AWAY, true, MOVE_ANIM_FRAME };
		case CharacterState::MoveLeft:
			return { ANIM_MOVE_LEFT, true, MOVE_ANIM_FRAME };
		case CharacterState::MoveRight:
			return { ANIM_MOVE_RIGHT, true, MOVE_ANIM_FRAME };
		case CharacterState::Shot:
			return { ANIM_SHOT, false, SHOT_ANIM_FRAME };
		case CharacterState::Missile:
			return { ANIM_MISSILE, false, MISSILE_ANIM_FRAME };
		case CharacterState::Fury:
			return { ANIM_FURY, false, FURY_ANIM_FRAME };
		case CharacterState::Beam:
			return { ANIM_BEAM, false, BEAM_ANIM_FRAME };
		case CharacterState::DodgeLeft:
			return { ANIM_DODGE_LEFT, false, DODGE_ANIM_FRAME };
		case CharacterState::DodgeRight:
			return { ANIM_DODGE_RIGHT, false, DODGE_ANIM_FRAME };
		case CharacterState::Dead:
			return { ANIM_DEAD, false, DEAD_ANIM_FRAME };
		case CharacterState::HitStun:
			return { ANIM_HIT_STUN, false, HIT_STUN_ANIM_FRAME };
		default:
			return { ANIM_WAIT, true, WAIT_ANIM_FRAME };
		}
	}
};