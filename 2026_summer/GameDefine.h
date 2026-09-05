#pragma once

namespace GameDefine
{
	// 画面の横幅
	constexpr int WIDTH = 1600;
	// 画面の縦幅
	constexpr int HEIGHT = 900;
	// 画面のカラービット
	constexpr int COLOR_BIT = 32;
	// 消失点の高さ
	constexpr float SCREEN_CENTER_Y = HEIGHT / 2;
	// 画面に表示できる最大パーティクル数
	constexpr int EFFECT_PARTICLE_LIMIT = 20000;
	// 照準(クロスヘア)の中心の隙間(ピクセル)。ここを起点にレイキャストの命中判定も拡張する
	constexpr int CROSSHAIR_GAP = 6;
}