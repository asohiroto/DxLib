#include "SceneMain.h"
#include "Player.h"
#include"Camera.h"

SceneMain::SceneMain():
	p_Player(nullptr),
	p_Camera(nullptr)
{
}

SceneMain::~SceneMain()
{
}

void SceneMain::Init()
{
	p_Player = std::make_shared<Player>();
	p_Camera = std::make_shared<Camera>();

	p_Player->Init();
	p_Camera->Init();
}

void SceneMain::End()
{

}

void SceneMain::Update()
{
	p_Player->Update();
	p_Camera->Update(p_Player);
}

void SceneMain::Draw()
{
	p_Player->Draw();
	p_Camera->Draw();
}
