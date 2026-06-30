#pragma once
#include"SceneBase.h"
#include"Map.h"
#include"RouteSearch.h"
#include"UnitManager.h";
#include"TurnManager.h"
#include"UIManager.h"

class MainScene : public SceneBase
{
public:
	MainScene();
	~MainScene() override;
	void Init() override;
	void Update() override;
	void Draw() override;

private:
	RouteSearch* p_RouteSearch;
	Map* p_Map;
	UnitManager* p_UnitManager;
	TurnManager* p_TurnManager;
	UIManager* p_UIManager;
};