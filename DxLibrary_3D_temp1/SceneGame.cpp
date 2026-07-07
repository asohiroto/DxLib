#include "SceneGame.h"
#include"Player.h"

SceneGame::SceneGame() :
	p_Player(nullptr)
{

}

SceneGame::~SceneGame()
{

}

void SceneGame::Init()
{
	p_Player = std::make_unique<Player>();
	p_Player->Init();
}

void SceneGame::Update()
{
	p_Player->Update();
}

void SceneGame::Draw()
{
	p_Player->Draw();
}