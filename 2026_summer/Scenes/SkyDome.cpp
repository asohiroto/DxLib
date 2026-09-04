#include "SkyDome.h"
#include <DxLib.h>

namespace
{
	// ドームの拡大率
	constexpr float DOME_SCALE = 5.0f;
}

SkyDome::SkyDome() :
	_domeH(-1)
{
}

SkyDome::~SkyDome()
{
}

void SkyDome::Init(int handle)
{
	_domeH = handle;
	MV1SetScale(_domeH, VGet(DOME_SCALE, DOME_SCALE, DOME_SCALE));
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
