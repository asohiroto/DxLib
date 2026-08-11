#include "MagicWall.h"

MagicWall::MagicWall() :
	_magicWall()
{
}

MagicWall::~MagicWall()
{
}

void MagicWall::Init()
{
	// 初期設定
	_magicWall.pos = VGet(0.0f, 0.0f, 0.0f);
	_magicWall.type = MagicType::MagicWall;
	_magicWall.isExist = false;
	_magicWall.isEnemy = false;
	_magicWall.color = 0xffffff;
}

void MagicWall::End()
{
}

void MagicWall::Update()
{
}

void MagicWall::Draw()
{
}
