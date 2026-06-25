#pragma once
#include"SceneBase.h"
#include"Map.h"
#include"RouteSearch.h"
#include"UnitManager.h";
/// <summary>
/// ゲームシーンクラス
/// </summary>
class MainScene : public SceneBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	MainScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~MainScene() override;

	/// <summary>
	/// 初期化
	/// </summary>
	void Init() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;

private:
	RouteSearch* p_RouteSearch;
	Map* p_Map;
	UnitManager* p_UnitManager;
};