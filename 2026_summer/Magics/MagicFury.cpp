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
	_magicFury.color = 0x0000ff;
	_magicFury.segmentStPos = VGet(0.0f, 0.0f, 0.0f);
	_magicFury.segmentEndPos = VGet(0.0f, 0.0f, 0.0f);
	_magicFury.isArrived = false;
	// ステータス決定
	_magicFury.damage = DAMAGE;
	_magicFury.chargeAmount = 0;
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

void MagicFury::GenerateFury(VECTOR pos, VECTOR front, bool isEnemy, std::shared_ptr<MagicManager> pManager)
{
	VECTOR tempPos = VAdd(pos, VGet(0.0f, FURY_HEIGHT, 0.0f));

	_magicFury.segmentEndPos = tempPos;
	_magicFury.segmentStPos = tempPos;
	_magicFury.isExist = true;
	_magicFury.isEnemy = isEnemy;
	_magicFury.moveDirection = VNorm(front);
	_magicFury.effectH = PlayEffekseer3DEffect(_magicFury.effectResourceH);
	SetScalePlayingEffekseer3DEffect(_magicFury.effectH, 1.0f, 1.5f, 1.0f);

	pManager->EntryList(_magicFury);
	pManager->LockOn();
}
