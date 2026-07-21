#pragma once
#include<DxLib.h>

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
	constexpr int GRID_SIZE = 200;
	// 足元に表示するグリッドの数
	constexpr int GRID_NUM = 30;

	// プレイヤーの挙動---------------------------------------
	// プレイヤーの当たり判定の半径
	constexpr float PLAYER_COL_RADIUS = 120.0f;
	// プレイヤーの当たり判定の線分の始点の補正値
	constexpr VECTOR PLAYER_SEGMENT_START_COR = { 0.0f, 100.0f, 0.0f };
	// プレイヤーの当たり判定の線分の終点の補正値
	constexpr VECTOR PLAYER_SEGMENT_FINISH_COR = { 0.0f, 450.0f, 0.0f };
	// 移動速度
	constexpr float MOVE_SPEED = 12.0f;
	// ダッシュ速度
	constexpr float DASH_SPEED = 24.0f;
	// 最大回避距離
	constexpr float DODGE_DISTANCE = 800.0f;
	// 回避速度
	constexpr float DODGE_SPEED = 20.0f;
	// 弱攻撃の当たり判定の生成高さの補正値
	constexpr VECTOR WEAK_ATTACK_HEIGHT_COR = { 0.0f, 400.0f, 0.0f };
	// 弱攻撃の当たり判定の生成位置の補正値
	constexpr float WEAK_ATTACK_POS_COR = 200.0f;
	// 弱攻撃の当たり判定の半径
	constexpr float WEAK_ATTACK_RADIUS = 100.0f;
	// ダメージ判定の間隔
	constexpr int DAMAGED_COOLDAWN = 60;
	// 攻撃の持続時間
	constexpr int ATTACKING_FRAME = 60;
	// 強攻撃の当たり判定の生成高さの補正値
	constexpr VECTOR STRONG_ATTACK_HEIGHT_COR = { 0.0f, 300.0f, 0.0f };
	// 強攻撃の当たり判定の生成位置の補正値
	constexpr float STRONG_ATTACK_POS_COR = 250.0f;
	// 強攻撃の当たり判定の半径
	constexpr float STRONG_ATTACK_RADIUS = 120.0f;
	// 弱攻撃ダメージ
	constexpr int WEAK_ATTACK_DAMAGE = 100;
	// 弱攻撃回復
	constexpr int WEAK_ATTACK_ABSORB = 100;
	// 強攻撃ダメージ
	constexpr int STRONG_ATTACK_DAMAGE = 200;
	// 強攻撃回復
	constexpr int STRONG_ATTACK_ABSORB = 50;
	// 弱攻撃のヒットストップフレーム
	constexpr int WEAK_HITSTOP_FRAME = 6;
	// 強攻撃のヒットストップフレーム
	constexpr int STRONG_HITSTOP_FRAME = 12;

	// ゲームパッド用の設定-----------------------------------
	// スティックのデッドゾーン
	constexpr float STICK_DEAD_ZONE = 150.0f;

}