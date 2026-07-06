#include "SceneGame.h"
#include"Player.h"
#include<DxLib.h>

namespace
{
	constexpr int kGridNum = 10;
	constexpr int kGridSize = 100;
}

SceneGame::SceneGame()
{

}

SceneGame::~SceneGame()
{

}

void SceneGame::Init()
{
	p_Player = std::make_shared<Player>();
	p_Player->Init();
}

void SceneGame::Update()
{
	SetCameraPositionAndAngle(VGet(0, 500, -1000), 0.5f, 0, 0);
	p_Player->Update();
}

void SceneGame::Draw() const
{
	DrawGrid();
	p_Player->Draw();
}

void SceneGame::DrawGrid() const
{
	const float lineStartX = kGridSize * -(kGridNum * 0.5f);
	const float lineEndX = -lineStartX;

	for (int z = 0; z <= kGridNum; z++)
	{
		const float lineZ = kGridSize * z - kGridSize * kGridNum * 0.5f;
		DrawLine3D(VGet(lineStartX, 0, lineZ), VGet(lineEndX, 0, lineZ), 0xffffff);
	}

	const float lineStartZ = kGridSize * -(kGridNum * 0.5f);
	const float lineEndZ = -lineStartZ;

	for (int x = 0; x <= kGridNum; x++)
	{
		const float lineX = kGridSize * x - kGridSize * kGridNum * 0.5f;
		DrawLine3D(VGet(lineX, 0, lineStartZ), VGet(lineX, 0, lineEndZ), 0xffffff);
	}
}