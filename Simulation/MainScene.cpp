#include"MainScene.h"
#include"Map.h"

MainScene::MainScene() :
	p_Map(nullptr),
	p_RouteSearch(nullptr),
	p_UnitManager(nullptr),
	p_TurnManager(nullptr)
{

}

MainScene::~MainScene()
{
	delete p_Map;
	delete p_RouteSearch;
	delete p_UnitManager;
	delete p_TurnManager;
}

void MainScene::Init()
{
	p_Map = new Map;
	p_RouteSearch = new RouteSearch;
	p_UnitManager = new UnitManager;
	p_TurnManager = new TurnManager;

	p_Map->Init();
	p_RouteSearch->Init();
	p_UnitManager->Init(p_RouteSearch);
	p_TurnManager->Init();
}

void MainScene::Update()
{
	p_Map->Update();
	p_RouteSearch->Update();
	p_UnitManager->Update(p_RouteSearch, p_TurnManager);
	p_TurnManager->Update();
}

void MainScene::Draw()
{
	p_Map->Draw();
	p_RouteSearch->Draw();
	p_UnitManager->Draw();
	p_TurnManager->Draw();
}