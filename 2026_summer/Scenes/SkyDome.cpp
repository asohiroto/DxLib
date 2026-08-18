#include "SkyDome.h"
#include <DxLib.h>

SkyDome::SkyDome() :
	_domeH(-1)
{
}

SkyDome::~SkyDome()
{
}

void SkyDome::Init()
{
	_domeH = MV1LoadModel("data/sunny_dome.mv1");
	MV1SetScale(_domeH, VGet(5.0f, 5.0f, 5.0f));
	MV1SetPosition(_domeH, VGet(0.0f, 0.0f, 0.0f));
}

void SkyDome::End()
{
}

void SkyDome::Update()
{
}

void SkyDome::Draw()
{
	MV1DrawModel(_domeH);
}
