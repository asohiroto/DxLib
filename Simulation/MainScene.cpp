#include"MainScene.h"
#include"Map.h"

MainScene::MainScene():
	p_Map(nullptr),
	p_Route(nullptr),
	p_Player(nullptr)
{

}

MainScene::~MainScene()
{

}

void MainScene::Init()
{
	p_Map = new Map;
	p_Route = new RouteSearch;
	p_Player = new PlayerUnit;

	p_Map->Init();
	p_Route->Init();
	p_Player->Init();
}

void MainScene::Update()
{
	p_Map->Update();
	p_Route->Update();
	p_Player->Update();
}

void MainScene::Draw()
{
	p_Map->Draw();
	p_Route->Draw();
	p_Player->Draw();
}