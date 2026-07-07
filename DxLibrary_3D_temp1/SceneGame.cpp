#include "SceneGame.h"
#include"Player.h"
#include"Camera.h"
#include<DxLib.h>

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
	p_Player->Update();
	p_Camera->Update(p_Player);
}

void SceneGame::Draw()
{
	p_Player->Draw();

	DrawSphere3D(VGet(320.0f, 200.0f, 0.0f), 80.0f, 32, GetColor(255, 0, 0), GetColor(255, 255, 255), TRUE);
}