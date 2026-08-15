#include "MagicBeam.h"
#include "MagicManager.h"

namespace
{
	// 半径
	constexpr float RADIUS = 100.0f;
	// 速度
	constexpr float SPEED = 90.0f;
	// 生成位置の補正用
	constexpr float POSITION_OFFSET = 50.0f;
	// 生成高さの補正用
	constexpr float HEIGHT_OFFSET = 300.0f;
	// ダメージ
	constexpr int DAMAGE = 75;
}

MagicBeam::MagicBeam() :
	_magicBeam()
{
}

MagicBeam::~MagicBeam()
{
}

void MagicBeam::Init()
{
	// 初期設定
	_magicBeam.radius = RADIUS;
	_magicBeam.speed = SPEED;
	_magicBeam.type = MagicType::MagicBeam;
	_magicBeam.isExist = false;
	_magicBeam.isEnemy = true;
	_magicBeam.color = 0x0000ff;
	_magicBeam.segmentStPos = VGet(0.0f, 0.0f, 0.0f);
	_magicBeam.segmentEndPos = VGet(0.0f, 0.0f, 0.0f);
	_magicBeam.damage = DAMAGE;
}

void MagicBeam::End()
{
}

void MagicBeam::Update()
{
}

void MagicBeam::Draw()
{
}

void MagicBeam::GenerateBeam(VECTOR pos, VECTOR front, bool isEnemy, std::shared_ptr<MagicManager> pManager)
{
	VECTOR tempPos = VAdd(pos, VScale(VNorm(front), POSITION_OFFSET));
	tempPos.y += HEIGHT_OFFSET;
	_magicBeam.segmentEndPos = tempPos;
	_magicBeam.segmentStPos = tempPos;
	_magicBeam.isExist = true;
	_magicBeam.isEnemy = isEnemy;
	_magicBeam.moveDirection = VNorm(front);

	pManager->EntryList(_magicBeam);
}
