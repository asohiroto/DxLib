#include "MagicManager.h"
#include "MagicMove.h"

MagicManager::MagicManager() :
	p_MagicMove(nullptr)
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

void MagicManager::Update()
{
	for (int i = 0; i < _playerMagics.size(); i++)
	{
		if (_playerMagics[i].isExist)
			p_MagicMove->ShotMove(_playerMagics[i]);
	}

	for (int i = 0; i < _enemyMagics.size(); i++)
	{
		if (_enemyMagics[i].isExist)
			p_MagicMove->ShotMove(_enemyMagics[i]);
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
		if (_playerMagics[i].isExist)
			DrawSphere3D(_playerMagics[i].pos, _playerMagics[i].radius, 16, 0xffffff, 0xffffff, true);
	}
}

void MagicManager::DrawEnemyMagic()
{
	for (int i = 0; i < _enemyMagics.size(); i++)
	{
		if (_enemyMagics[i].isExist)
			DrawSphere3D(_enemyMagics[i].pos, _enemyMagics[i].radius, 16, 0xffffff, 0xffffff, true);
	}
}