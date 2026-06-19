#pragma once
#include"SceneBase.h"
#include"Map.h"
#include"RouteSearch.h"

/// <summary>
/// ゲームシーンクラス
/// </summary>
class MainScene : public SceneBase
{
public:
	RouteSearch* p_Route;
	Map* p_Map;

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
};