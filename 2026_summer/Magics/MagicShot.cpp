#include "MagicShot.h"
#include "MagicManager.h"
#include <DxLib.h>
#include <EffekseerForDXLib.h>

namespace
{
	// 当たり判定の半径
	constexpr float RADIUS = 100.0f;
	// 進む速さ
	constexpr float SPEED = 200.0f;
	// 生成位置の補正用
	constexpr float POSITION_OFFSET = 50.0f;
	// 生成高さの補正用
	constexpr float HEIGHT_OFFSET = 300.0f;
	// ダメージ
	constexpr int DAMAGE = 50;
	// 消費魔力
	constexpr float USE_MP = 10.0f;
	// 必殺技チャージ量
	constexpr int CHARGE_AMOUNT = 5;
	// ヒットストップするフレーム数
	constexpr int HIT_STOP_FRAME = 1;
	// 当たり判定描画用の色
	constexpr int COLOR = 0xffffff;
}

MagicShot::MagicShot() :
	_magicShot()
{
}

MagicShot::~MagicShot()
{
}

void MagicShot::Init()
{
	// 初期設定
	_magicShot.pos = VGet(0.0f, 0.0f, 0.0f);
	_magicShot.radius = RADIUS;
	_magicShot.speed = SPEED;
	_magicShot.type = MagicType::MagicShot;
	_magicShot.isExist = false;
	_magicShot.isEnemy = false;
	_magicShot.color = COLOR;
	// ステータス決定
	_magicShot.damage = DAMAGE;
	_magicShot.useMp = USE_MP;
	_magicShot.chargeAmount = CHARGE_AMOUNT;
	_magicShot.hitStopFrame = HIT_STOP_FRAME;
}

void MagicShot::End()
{
}

void MagicShot::Update()
{
}

void MagicShot::Draw()
{
}

void MagicShot::GenerateShot(VECTOR pos, VECTOR front, bool isEnemy, const std::shared_ptr<MagicManager>& pManager)
{
	VECTOR tempPos = VAdd(pos, VScale(VNorm(front), POSITION_OFFSET));
	tempPos.y += HEIGHT_OFFSET;
	_magicShot.pos = tempPos;
	_magicShot.isExist = true;
	_magicShot.isEnemy = isEnemy;
	_magicShot.moveDirection = VNorm(front);
	_magicShot.effectH = PlayEffekseer3DEffect(_magicShot.effectResourceH);

	pManager->EntryList(_magicShot);
}