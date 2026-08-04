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
	DrawCircle(WIDTH / 2, HEIGHT / 2, 0xffffff, 5, 5);
}
