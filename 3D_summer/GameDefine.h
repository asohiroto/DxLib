#pragma once

namespace GameDefine
{
	// 画面の基礎設定-----------------------------------------
	// 画面の横幅
	constexpr int WIDTH = 1280;
	// 画面の縦幅
	constexpr int HEIGHT = 720;
	// カラービット
	constexpr int COLOR_BIT = 32;

	// カメラの諸々-------------------------------------------
	// カメラの水平方向の速度
	constexpr float CAMERA_YAW_SPEED = 0.06f;
	// カメラの垂直方向の速度
	constexpr float CAMERA_PITCH_SPEED = 0.06f;
	// カメラの垂直方向の回転制限
	constexpr float CAMERA_PITCH_LIMIT = 1.2f;
	// 描画距離
	constexpr float CAMERA_DISTANCE = -1750.0f;
	// 注視点の高さの補整度
	constexpr float CAMERA_TARGET_HEIGHT = -150.0f;
	// 注視点をプレイヤーの向いている向きに補整
	constexpr float CAMERA_TARGET_FORWARD_OFFSET = -150.0f;
	// カメラの注視点切り替え用線形補完度
	constexpr float CAMERA_FORWARD_LERP_RATE = 0.03f;
	// カメラの球面線形補間度
	constexpr float CAMERA_SLERP_RATE = 0.015f;
	// カメラの線形補間度
	constexpr float CAMERA_LERP_RATE = 1.0f;

	// デバッグ用の表示---------------------------------------
	// 足元に表示するグリッドの大きさ
	constexpr int GRID_SIZE = 100;
	// 足元に表示するグリッドの数
	constexpr int GRID_NUM = 20;

	// プレイヤーの挙動---------------------------------------
	// 重力加速度
	constexpr float GRAVITY_ACCEL = 3.0f;
	// ジャンプ時の初速度
	constexpr float JUMP_SPEED = 30.0f;
	// 移動速度
	constexpr float MOVE_SPEED = 10.0f;

	// ゲームパッド用の設定-----------------------------------
	// スティックのデッドゾーン
	constexpr float STICK_DEAD_ZONE = 150.0f;

}