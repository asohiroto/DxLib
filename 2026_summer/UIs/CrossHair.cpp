#include "CrossHair.h"
#include "GameDefine.h"
#include <DxLib.h>

using namespace GameDefine;

CrossHair::CrossHair()
{
}

CrossHair::~CrossHair()
{
}

void CrossHair::Init()
{
}

void CrossHair::End()
{
}

void CrossHair::Update()
{
}

void CrossHair::Draw()
{
	// クロスヘアを書く
	DrawCircle(WIDTH / 2, SCREEN_CENTER_Y - 150.0f, 5, 0xffffff, true);
}