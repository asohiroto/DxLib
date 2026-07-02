#include"MainScene.h"
#include"Map.h"

MainScene::MainScene() :
	p_Map(nullptr),
	p_RouteSearch(nullptr),
	p_UnitManager(nullptr),
	p_TurnManager(nullptr),
	p_UIManager(nullptr),
	p_BaseManager(nullptr)
{

}

MainScene::~MainScene()
{
	delete p_Map;
	delete p_RouteSearch;
	delete p_UnitManager;
	delete p_TurnManager;
	delete p_UIManager;
	delete p_BaseManager;
}

void MainScene::Init()
{
	p_Map = new Map;
	p_RouteSearch = new RouteSearch;
	p_UnitManager = new UnitManager;
	p_TurnManager = new TurnManager;
	p_UIManager = new UIManager;
	p_BaseManager = new BaseManager;

	p_Map->Init();
	p_RouteSearch->Init();
	p_UnitManager->Init(p_RouteSearch);
	p_TurnManager->Init();
	p_UIManager->Init();
	p_BaseManager->Init();
}

void MainScene::Update()
{
	p_Map->Update();
	p_RouteSearch->Update();
	p_UnitManager->Update(p_RouteSearch, p_TurnManager);
	p_TurnManager->Update();
	p_UIManager->Update(p_UnitManager->GetPlayerPointer(), p_UnitManager->GetEnemyPointer(), p_RouteSearch, p_TurnManager, p_UnitManager);
	p_BaseManager->Update(p_RouteSearch, p_UIManager, p_UnitManager);
}

void MainScene::Draw()
{
	p_Map->Draw();
	p_RouteSearch->Draw();
	p_TurnManager->Draw();
	p_UnitManager->Draw(p_TurnManager);
	p_UIManager->Draw();
	p_BaseManager->Draw();
}