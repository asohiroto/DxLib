#pragma once
#include"SceneBase.h"

class RouteSearch;
class Map;
class UnitManager;
class TurnManager;
class UIManager;
class BaseManager;

class MainScene : public SceneBase
{
public:
	MainScene(SceneManager& _sceneManager);
	~MainScene() override;
	void Init() override;
	void End() override;
	void Update() override;
	void Draw() override;

private:
	RouteSearch* p_RouteSearch;
	Map* p_Map;
	UnitManager* p_UnitManager;
	TurnManager* p_TurnManager;
	UIManager* p_UIManager;
	BaseManager* p_BaseManager;
};