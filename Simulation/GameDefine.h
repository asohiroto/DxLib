#pragma once

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
	// 画面の横幅
	constexpr int WIDTH = 1280;
	// 画面の縦幅
	constexpr int HEIGHT = 720;
	// ビット深度
	constexpr int DEPTH = 32;
	// ノードの列の最大数
	constexpr int NODE_HEIGHT = 36;
	// ノードの行の最大数
	constexpr int NODE_WIDTH = 64;
	// ノードのサイズ
	constexpr int NODE_SIZE = 20;
	// 自軍拠点の座標
	constexpr float MY_BASE_X = 3, MY_BASE_Y = 17;
	// 敵軍拠点の座標
	constexpr float ENEMY_BASE_X = 61, ENEMY_BASE_Y = 17;
}