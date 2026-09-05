#include "MagicFury.h"
#include "MagicManager.h"
#include <EffekseerForDXLib.h>

namespace
{
	// 半径
	constexpr float RADIUS = 100.0f;
	// 速度
	constexpr float SPEED = 120.0f;
	// 生成位置の補正用
	constexpr float POSITION_OFFSET = 50.0f;
	// 生成高さの補正用
	constexpr float HEIGHT_OFFSET = 300.0f;
	// マジックフューリー生成の高さ
	constexpr float FURY_HEIGHT = 2400.0f;
	// ダメージ
	constexpr int DAMAGE = 200;
	// ヒットストップするフレーム数
	constexpr int HIT_STOP_FRAME = 2;
	// 当たり判定描画用の色
	constexpr int COLOR = 0x0000ff;
	// 発生エフェクトのY方向スケール
	constexpr float EFFECT_SCALE_Y = 1.5f;
}

MagicFury::MagicFury() :
	_magicFury()
{
}

MagicFury::~MagicFury()
{
}

void MagicFury::Init()
{
	// 初期設定
	_magicFury.radius = RADIUS;
	_magicFury.speed = SPEED;
	_magicFury.type = MagicType::MagicFury;
	_magicFury.isExist = false;
	_magicFury.isEnemy = true;
	_magicFury.color = COLOR;
	_magicFury.segmentStPos = VGet(0.0f, 0.0f, 0.0f);
	_magicFury.segmentEndPos = VGet(0.0f, 0.0f, 0.0f);
	_magicFury.isArrived = false;
	// ステータス決定
	_magicFury.damage = DAMAGE;
	_magicFury.chargeAmount = 0;
	_magicFury.hitStopFrame = HIT_STOP_FRAME;
}

void MagicFury::End()
{
}

void MagicFury::Update()
{
}

void MagicFury::Draw()
{
}

void MagicFury::GenerateFury(VECTOR pos, VECTOR front, bool isEnemy, const std::shared_ptr<MagicManager>& pManager)
{
	// 上空に発生位置を設定（後で目標に向かって落下する）
	VECTOR tempPos = VAdd(pos, VGet(0.0f, FURY_HEIGHT, 0.0f));

	_magicFury.segmentEndPos = tempPos;
	_magicFury.segmentStPos = tempPos;
	_magicFury.isExist = true;
	_magicFury.isEnemy = isEnemy;
	_magicFury.moveDirection = VNorm(front);
	_magicFury.effectH = PlayEffekseer3DEffect(_magicFury.effectResourceH);
	// エフェクトを縦方向に拡大
	SetScalePlayingEffekseer3DEffect(_magicFury.effectH, 1.0f, EFFECT_SCALE_Y, 1.0f);

	pManager->EntryList(_magicFury);
	// フューリー発動中は敵をロックオンする
	pManager->LockOn();
}
