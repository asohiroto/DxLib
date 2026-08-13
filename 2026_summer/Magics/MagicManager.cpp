#include "MagicManager.h"
#include "MagicMove.h"

namespace
{
	// 高さ補正用
	constexpr float ENEMY_HEIGHT_OFFSET = 300.0f;
	// マジックフューリーの目的地の深さ
	constexpr float FURY_DIPTH = 1200.0f;
}

MagicManager::MagicManager() :
	p_MagicMove(nullptr),
	_enemyLock(false),
	_enePos(VGet(0.0f, 0.0f, 0.0f))
{
}

MagicManager::~MagicManager()
{
}

void MagicManager::Init()
{
	p_MagicMove = std::make_shared<MagicMove>();
}

void MagicManager::End()
{
}

void MagicManager::Update(VECTOR playerPos, VECTOR enemyPos)
{
	_enePos = enemyPos;

	for (int i = 0; i < _playerMagics.size(); i++)
	{
		if (_playerMagics[i].isExist)
		{
			if (_playerMagics[i].type == MagicBase::MagicType::MagicShot)
				p_MagicMove->ShotMove(_playerMagics[i]);
			else if (_playerMagics[i].type == MagicBase::MagicType::MagicMissile)
				p_MagicMove->MissileMove(_playerMagics[i], enemyPos);
			else if (_playerMagics[i].type == MagicBase::MagicType::MagicFury)
				p_MagicMove->FuryMove(_playerMagics[i], VAdd(enemyPos, VGet(0.0f, -FURY_DIPTH, 0.0f)));
		}

		if (_playerMagics[i].isArrived)
		{
			_enemyLock = false;
			_playerMagics[i].isExist = false;
		}
	}

	for (int i = 0; i < _enemyMagics.size(); i++)
	{
		if (_enemyMagics[i].isExist)
		{
			if (_enemyMagics[i].type == MagicBase::MagicType::MagicShot)
				p_MagicMove->ShotMove(_enemyMagics[i]);
			else if (_enemyMagics[i].type == MagicBase::MagicType::MagicMissile)
				p_MagicMove->MissileMove(_enemyMagics[i], playerPos);
			else if (_enemyMagics[i].type == MagicBase::MagicType::MagicBeam)
				p_MagicMove->BeamMove(_enemyMagics[i], playerPos, VAdd(enemyPos, VGet(0.0f, ENEMY_HEIGHT_OFFSET, 0.0f)));
		}
	}
}

void MagicManager::Draw()
{
	DrawPlayerMagic();
	DrawEnemyMagic();
}

void MagicManager::EntryList(MagicBase::MagicData data)
{
	if (data.isEnemy) _enemyMagics.push_back(data);
	else _playerMagics.push_back(data);
}

void MagicManager::DrawPlayerMagic()
{
	for (int i = 0; i < _playerMagics.size(); i++)
	{
		DrawMagic(_playerMagics[i]);
	}
}

void MagicManager::DrawEnemyMagic()
{
	for (int i = 0; i < _enemyMagics.size(); i++)
	{
		DrawMagic(_enemyMagics[i]);
	}
}

void MagicManager::RemoveList()
{
	std::erase_if(_playerMagics, [](const MagicBase::MagicData& data) {return !data.isExist; });
	std::erase_if(_enemyMagics, [](const MagicBase::MagicData& data) {return !data.isExist; });
}

void MagicManager::DrawMagic(MagicBase::MagicData data)
{
	if (data.isExist)
	{
		if (data.type == MagicBase::MagicType::MagicBeam || data.type == MagicBase::MagicType::MagicFury)
			DrawCapsule3D(data.segmentStPos, data.segmentEndPos, data.radius, 16, data.color, data.color, true);
		else
			DrawSphere3D(data.pos, data.radius, 16, data.color, data.color, true);
	}
}