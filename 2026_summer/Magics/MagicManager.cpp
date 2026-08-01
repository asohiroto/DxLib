#include "MagicManager.h"

MagicManager::MagicManager()
{
}

MagicManager::~MagicManager()
{
}

void MagicManager::Init()
{
}

void MagicManager::End()
{
}

void MagicManager::Update()
{
}

void MagicManager::Draw()
{
}

void MagicManager::EntryList(MagicBase::MagicData data)
{
	if (data.isEnemy) _enemyMagics.push_back(data);
	else _playerMagics.push_back(data);
}
