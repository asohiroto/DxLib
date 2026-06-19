#pragma once
#include"AsoDxLib/Vec2.h"
#include"GameDefine.h"

/// <summary>
/// マップクラス
/// </summary>
class Map
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	Map();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Map();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private:
	/// <summary>
	/// マップデータを読み込む
	/// </summary>
	void LoadMapData();

	/// <summary>
	/// マップの描画
	/// </summary>
	void DrawMapChip();

	int _mapHandle;
	int _chipData[GameDefine::NODE_HEIGHT][GameDefine::NODE_WIDTH];
	
};