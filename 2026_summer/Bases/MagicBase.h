#pragma once
#include <DxLib.h>

class MagicBase
{
public:
	// 使う魔法の種類
	enum class MagicType
	{
		MagicShot,
		MagicMissile,
		MagicBeam,
		MagicFury
	};

	// 魔法のデータ
	struct MagicData
	{
		// 座標
		VECTOR pos;
		// 当たり判定の半径
		float radius;
		// 魔法の進む速度
		float speed;
		// 魔法の種類
		MagicType type;
		// 存在するか
		bool isExist;
		// 敵が使用したモノか
		bool isEnemy;
		// 移動させる方向
		VECTOR moveDirection;
		// 移動距離
		float movedDistance;
		// 当たり判定描画用の色設定
		int color;
		// 初速度
		VECTOR velo;
		// 線分の始点
		VECTOR segmentStPos;
		// 線分の終点
		VECTOR segmentEndPos;
		// 目的地に到着したか
		bool isArrived;
		// ダメージ
		int damage;
		// 消費魔力
		float useMp;
		// エフェクトのハンドル
		int effectResourceH;
		// 再生中のエフェクトのハンドル
		int effectH;
		// チャージ中のカウント
		int chargeCount;
		// 生成カウンタ
		int existCount;
		// マジックビームが目的とする位置
		VECTOR beamTargetPos;
		// ヒット時に必殺技をチャージする量
		int chargeAmount;
	};

public:
	MagicBase() {};
	virtual ~MagicBase() {};
	virtual void Init() {};
	virtual void End() {};
	virtual void Update() {};
	virtual void Draw() {};
};