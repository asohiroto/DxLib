#include"MainScene.h"
#include"Map.h"

MainScene::MainScene():
	p_Map(nullptr),
	p_Route(nullptr)
{

}

MainScene::~MainScene()
{

}

void MainScene::Init()
{
	p_Map = new Map;
	p_Route = new RouteSearch;

	p_Map->Init();
	p_Route->Init();
}

void MainScene::Update()
{
	p_Map->Update();
	p_Route->Update();
}

void MainScene::Draw()
{
	p_Map->Draw();
	p_Route->Draw();
}