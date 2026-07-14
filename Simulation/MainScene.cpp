#include"MainScene.h"
#include"RouteSearch.h"
#include"UnitManager.h";
#include"TurnManager.h"
#include"UIManager.h"
#include"Basemanager.h"
#include"Map.h"

MainScene::MainScene(SceneManager& _sceneManager) :
	SceneBase(_sceneManager),
	p_Map(nullptr),
	p_RouteSearch(nullptr),
	p_UnitManager(nullptr),
	p_TurnManager(nullptr),
	p_UIManager(nullptr),
	p_BaseManager(nullptr),
	_bgH(-1)
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
	_bgH = LoadGraph("data/MainBg.jpg");

	p_Map = new Map;
	p_RouteSearch = new RouteSearch;
	p_UnitManager = new UnitManager;
	p_TurnManager = new TurnManager;
	p_UIManager = new UIManager;
	p_BaseManager = new BaseManager;

	p_Map->Init();
	p_RouteSearch->Init();
	p_UnitManager->Init(p_RouteSearch, _sceneManager);
	p_TurnManager->Init();
	p_UIManager->Init();
	p_BaseManager->Init();
}

void MainScene::End()
{
	DeleteGraph(_bgH);
}

void MainScene::Update()
{
	p_Map->Update();
	p_RouteSearch->Update();
	p_UnitManager->Update(p_RouteSearch, p_TurnManager, _sceneManager);
	p_TurnManager->Update();
	p_UIManager->Update(p_UnitManager->GetPlayerPointer(), p_UnitManager->GetEnemyPointer(), p_RouteSearch, p_TurnManager, p_UnitManager);
	p_BaseManager->Update(p_RouteSearch, p_UIManager, p_UnitManager, p_TurnManager);
}

void MainScene::Draw()
{
	DrawGraph(0, 0, _bgH, false);

	p_Map->Draw();
	p_RouteSearch->Draw();
	p_TurnManager->Draw();
	p_BaseManager->Draw(p_UIManager);
	p_UnitManager->Draw(p_TurnManager);
	p_UIManager->Draw(p_RouteSearch);
}