#pragma once

// 地形情報
enum class TileType
{
	Plain		= 0,
	MyBase		= 1,
	River		= 2,
	Forest		= 3,
	EnemyBase	= 4,
	Mountain	= 5
};

namespace GameDefine
{
	// ゲーム画面関係-----------------------------------------
	// 画面の横幅
	constexpr int WIDTH = 1600;
	// 画面の縦幅
	constexpr int HEIGHT = 900;
	// ビット深度
	constexpr int DEPTH = 32;
	
	// マップ関係--------------------------------------------
	// ノードの列の最大数
	constexpr int NODE_HEIGHT = 36;
	// ノードの行の最大数
	constexpr int NODE_WIDTH = 64;
	// ノードのサイズ
	constexpr int NODE_SIZE = 20;
	// マップの横幅
	constexpr int MAP_WIDTH = NODE_WIDTH * NODE_SIZE;
	// マップの縦幅
	constexpr int MAP_HEIGHT = NODE_HEIGHT * NODE_SIZE;
	// 自軍拠点の座標
	constexpr float MY_BASE_X = 2, MY_BASE_Y = 17;
	// 敵軍拠点の座標
	constexpr float ENEMY_BASE_X = 61, ENEMY_BASE_Y = 17;

	// UI関係------------------------------------------------
	// セレクトボタンのX座標
	constexpr int SELECTING_X = MAP_WIDTH;
	// セレクトボタンのY座標
	constexpr int SELECTING_Y = 400;
	// セレクトボタンの横幅
	constexpr int SELLECTING_WIDTH = 200;
	// セレクトボタンの縦幅
	constexpr int SELLECTING_HEIGHT = 20;
	// ターン進行ボタンのX座標
	constexpr int TURNBUTTON_X = MAP_WIDTH;
	// ターン進行ボタンのY座標
	constexpr int TURNBUTTON_Y = MAP_HEIGHT;
	// ターン進行ボタンの縦幅
	constexpr int TURNBUTTON_WIDTH = 200;
	// ターン進行ボタンの横幅
	constexpr int TURNBUTTON_HEIGHT = 40;
	// ユニット情報パネルのX座標
	constexpr int DATA_X = MAP_WIDTH;
	// ユニット情報パネルのY座標
	constexpr int DATA_Y = 200;
	// ユニット情報パネルの横幅
	constexpr int DATA_WIDTH = 200;
	// ユニット情報パネルの縦幅
	constexpr int DATA_HEIGHT = 100;
	// アクションメニューのX座標
	constexpr int ACTION_X = MAP_WIDTH;
	// アクションメニューのY座標
	constexpr int ACTION_Y = 500;
	// アクションメニューの横幅
	constexpr int ACTION_WIDTH = 200;
	// アクションメニューの縦幅
	constexpr int ACTION_HEIGHT = 180;

	// ユニット関係-----------------------------------------
	// 行動間隔
	constexpr int MOVE_SPAN = 10;
}