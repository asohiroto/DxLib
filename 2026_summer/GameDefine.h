#pragma once

namespace GameDefine
{
	// 画面の横幅
	constexpr int WIDTH = 1600;
	// 画面の縦幅
	constexpr int HEIGHT = 900;
	// 画面のカラービット
	constexpr int COLOR_BIT = 32;
	// 画面中央のX座標
	constexpr float SCREEN_CENTER_X = WIDTH / 2;
	// 消失点の高さ
	constexpr float SCREEN_CENTER_Y = HEIGHT / 2;
	// 画面に表示できる最大パーティクル数
	constexpr int EFFECT_PARTICLE_LIMIT = 20000;
	// 照準(クロスヘア)の中心の隙間(ピクセル)。ここを起点にレイキャストの命中判定も拡張する
	constexpr int CROSSHAIR_GAP = 6;
	// 外周セーフゾーン(横5%)のピクセル幅。UI配置時はここより内側に収める
	constexpr int SAFE_ZONE_MARGIN_X = WIDTH * 5 / 100;
	// 外周セーフゾーン(縦5%)のピクセル幅。UI配置時はここより内側に収める
	constexpr int SAFE_ZONE_MARGIN_Y = HEIGHT * 5 / 100;
}