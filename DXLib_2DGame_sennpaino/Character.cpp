#include"DXLib.h"
#include "Character.h"

Character::Character()
{

}

Character::~Character()
{

}

void Character::Init()
{

}

void Character::Update()
{


}

void Character::Draw(float x, float y, float angle, int handle)
{
	DrawRotaGraph(static_cast<int>(x), static_cast<int>(y), 1.0f, angle, handle, TRUE, FALSE);
}


