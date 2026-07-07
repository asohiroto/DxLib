#include "SceneGame.h"
#include"Player.h"
#include"Camera.h"
#include"GameDefine.h"
#include<DxLib.h>

using namespace GameDefine;

SceneGame::SceneGame() :
	p_Player(nullptr),
	p_Camera(nullptr)
{

}

SceneGame::~SceneGame()
{

}

void SceneGame::Init()
{
	p_Player = std::make_shared<Player>();
	p_Player->Init();
	p_Camera = std::make_shared<Camera>();
	p_Camera->Init();
}

void SceneGame::Update()
{
	p_Player->Update(p_Camera->GetCameraYaw());
	p_Camera->Update(p_Player);
}

void SceneGame::Draw()
{
	p_Player->Draw();
	DrawGrid();
}

void SceneGame::DrawGrid() const
{
	const float lineStartX = GRID_SIZE * -(GRID_NUM * 0.5f);
	const float lineEndX = -lineStartX;

	for (int z = 0; z <= GRID_NUM; z++)
	{
		const float lineZ = GRID_SIZE * z - GRID_SIZE * GRID_NUM * 0.5f;
		DrawLine3D(VGet(lineStartX, 0, lineZ), VGet(lineEndX, 0, lineZ), 0xffffff);
	}

	const float lineStartZ = GRID_SIZE * -(GRID_NUM * 0.5f);
	const float lineEndZ = -lineStartZ;

	for (int x = 0; x <= GRID_NUM; x++)
	{
		const float lineX = GRID_SIZE * x - GRID_SIZE * GRID_NUM * 0.5f;
		DrawLine3D(VGet(lineX, 0, lineStartZ), VGet(lineX, 0, lineEndZ), 0xffffff);
	}
}