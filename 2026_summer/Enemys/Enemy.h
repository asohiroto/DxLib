#pragma once
#include "Bases/Character.h"

class Enemy : public Character
{
public:
	Enemy();
	virtual ~Enemy() override;
	void Init() override;
	void End() override;
	void Update() override;
	void Draw() override;

private:
	CharacterData _tempEnemyUnit;
};