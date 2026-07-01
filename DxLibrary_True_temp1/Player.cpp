#include "Player.h"
#include"DxLib.h"

Player::Player() :
	_posX(0),
	_posY(0)
{

}

Player::~Player()
{

}

void Player::Init()
{
	_posX = kStartX;
	_posY = kStartY;
}

void Player::Update()
{
	if ((GetJoypadInputState(DX_INPUT_KEY_PAD1) & PAD_INPUT_UP) != 0)
	{
		_posY -= 4;
	}
	if ((GetJoypadInputState(DX_INPUT_KEY_PAD1) & PAD_INPUT_DOWN) != 0)
	{
		_posY += 4;
	}
	if ((GetJoypadInputState(DX_INPUT_KEY_PAD1) & PAD_INPUT_RIGHT) != 0)
	{
		_posX += 4;
	}
	if ((GetJoypadInputState(DX_INPUT_KEY_PAD1) & PAD_INPUT_LEFT) != 0)
	{
		_posX -= 4;
	}
}

void Player::Draw()
{
	DrawCircle(_posX, _posY, kRadius, 0x216531, true);
}