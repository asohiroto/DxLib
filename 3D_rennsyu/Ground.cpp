#include "Ground.h"
#include<cassert>

namespace
{
	const char* kModelPath = "data/Cube_Bricks.mv1";
}

Ground::Ground() :
	GameObject(),
	_modelH(-1)
{

}

Ground::~Ground()
{
	MV1DeleteModel(_modelH);
}

void Ground::Init()
{
	_modelH = MV1LoadModel(kModelPath);
	assert(_modelH != -1);

	MV1SetupCollInfo(_modelH, -1);
	SetCollision(true, CollisionType::Poly, 0.0f, _modelH);
}

void Ground::Update()
{

}

void Ground::Draw() const
{
	MV1DrawModel(_modelH);
}