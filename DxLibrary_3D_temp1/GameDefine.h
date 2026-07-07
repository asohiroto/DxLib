#pragma once

namespace GameDefine
{
	// カメラの水平方向の速度
	constexpr float CAMERA_YAW_SPEED = 0.02f;
	// カメラの垂直方向の速度
	constexpr float CAMERA_PITCH_SPEED = 0.02f;
	// カメラの垂直方向の回転制限
	constexpr float CAMERA_PITCH_LIMIT = 1.5f;
	// 足元に表示するグリッドの大きさ
	constexpr int GRID_SIZE = 100;
	// 足元に表示するグリッドの数
	constexpr int GRID_NUM = 20;
}