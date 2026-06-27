#include"MainScene.h"
#include"Map.h"

MainScene::MainScene() :
	p_Map(nullptr),
	p_RouteSearch(nullptr),
	p_UnitManager(nullptr)
{

}

MainScene::~MainScene()
{

}

void MainScene::Init()
{
	p_Map = new Map;
	p_RouteSearch = new RouteSearch;
	p_UnitManager = new UnitManager;

	p_Map->Init();
	p_RouteSearch->Init();
	p_UnitManager->Init(p_RouteSearch);
}

void MainScene::Update()
{
	p_Map->Update();
	p_RouteSearch->Update();
	p_UnitManager->Update();
}

void MainScene::Draw()
{
	p_Map->Draw();
	p_RouteSearch->Draw();
	p_UnitManager->Draw();
}