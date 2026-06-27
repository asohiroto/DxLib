#pragma once
#include"AsoDxLib/Vec2.h"
#include"GameDefine.h"

class Map
{
public:
	Map();
	~Map();
	void Init();
	void Update();
	void Draw();

private:
	// マップデータを読み込む
	void LoadMapData();
	// マップの描画
	void DrawMapChip();

private:
	// マップの見た目
	int _mapHandle;
	// グリッドの地形情報保存用二次元配列
	int _chipData[GameDefine::NODE_HEIGHT][GameDefine::NODE_WIDTH];
};