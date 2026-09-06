#include "UIManager.h"
#include "EnemyHpBar.h"
#include "PlayerHpBar.h"
#include "PlayerMpBar.h"
#include "PlayerUltGauge.h"
#include "GameDefine.h"
#include <DxLib.h>

namespace
{
	// クロスヘアの色(白)
	constexpr int CROSSHAIR_COLOR = 0xffffff;
	// 敵にヒットした際のクロスヘアの色(赤)
	constexpr int CROSSHAIR_HIT_COLOR = 0xff0000;
	// クロスヘアの太さ
	constexpr int CROSSHAIR_THICKNESS = 2;
	// クロスヘアの腕の長さ
	constexpr int CROSSHAIR_ARM_LENGTH = 12;
	// ヒット時にクロスヘアを赤く表示し続けるフレーム数
	constexpr int CROSSHAIR_HIT_FRAME = 15;

	// 操作ガイドの背景(外周5%セーフゾーンの内側、画面左下に小さく配置)
	constexpr int GUIDE_BOX_X = GameDefine::SAFE_ZONE_MARGIN_X;
	constexpr int GUIDE_BOX_WIDTH = 440;
	constexpr int GUIDE_BOX_HEIGHT = 150;
	// 背景を少し上にずらす分のオフセット
	constexpr int GUIDE_BOX_Y_OFFSET = -20;
	constexpr int GUIDE_BOX_Y = GameDefine::HEIGHT - GameDefine::SAFE_ZONE_MARGIN_Y - GUIDE_BOX_HEIGHT + GUIDE_BOX_Y_OFFSET;
	// 半透明の紫背景にする
	constexpr int GUIDE_BOX_COLOR = 0x6a0dad;
	constexpr int GUIDE_BOX_ALPHA = 180;
	// 操作ガイドのボタンアイコンのサイズ(1.5倍)
	constexpr int GUIDE_ICON_SIZE = 39;
	// 操作ガイドの文字サイズ・色
	constexpr int GUIDE_FONT_SIZE = 20;
	constexpr int GUIDE_TEXT_COLOR = 0xffffff;
	// 各行のアイコン・文字の左端座標
	constexpr int GUIDE_ICON_X = GUIDE_BOX_X + 10;
	constexpr int GUIDE_TEXT_X = GUIDE_ICON_X + GUIDE_ICON_SIZE + 8;
	// アイコンに対して文字を少し下にずらし、縦方向の中心を合わせる
	constexpr int GUIDE_TEXT_Y_OFFSET = 9;
	// 各行のY座標(アイコンサイズ+隙間で積み上げ)
	constexpr int GUIDE_ROW1_Y = GUIDE_BOX_Y + 10;
	constexpr int GUIDE_ROW2_Y = GUIDE_ROW1_Y + GUIDE_ICON_SIZE + 6;
	constexpr int GUIDE_ROW3_Y = GUIDE_ROW2_Y + GUIDE_ICON_SIZE + 6;
	// デフォルトのフォントサイズ
	constexpr int NORMAL_FONT_SIZE = 20;
}

UIManager::UIManager() :
	p_EneHpBar(nullptr),
	p_PlayHpBar(nullptr),
	p_PlayMpBar(nullptr),
	p_PlayUltGauge(nullptr),
	_crosshairRedTimer(0),
	_guideYButtonH(-1),
	_guideBButtonH(-1),
	_guideAButtonH(-1)
{
}

UIManager::~UIManager()
{
}

void UIManager::Init(float eneMaxHp, float playMaxHp,
	float playMaxMp, float maxCharge, int pHpH, int eHpH, int ultH)
{
	p_EneHpBar = std::make_shared<EnemyHpBar>();
	p_EneHpBar->Init(eneMaxHp, eHpH);

	p_PlayHpBar = std::make_shared<PlayerHpBar>();
	p_PlayHpBar->Init(playMaxHp, pHpH);

	p_PlayMpBar = std::make_shared<PlayerMpBar>();
	p_PlayMpBar->Init(playMaxMp);

	p_PlayUltGauge = std::make_shared<PlayerUltGauge>();
	p_PlayUltGauge->Init(maxCharge, ultH);

	_guideYButtonH = LoadGraph("data/GUI/xbox_Y.png");
	_guideBButtonH = LoadGraph("data/GUI/xbox_B.png");
	_guideAButtonH = LoadGraph("data/GUI/xbox_A.png");
}

void UIManager::End()
{
}

void UIManager::Update(float eneNowHp, float playNowHp, float playNowMp, float nowCharge, bool isEnemyHit)
{
	p_EneHpBar->Update(eneNowHp);
	p_PlayHpBar->Update(playNowHp);
	p_PlayMpBar->Update(playNowMp);
	p_PlayUltGauge->Update(nowCharge);

	if (isEnemyHit) _crosshairRedTimer = CROSSHAIR_HIT_FRAME;
	else if (_crosshairRedTimer > 0) _crosshairRedTimer--;
}

void UIManager::Draw()
{
	p_EneHpBar->Draw();
	p_PlayHpBar->Draw();
	//p_PlayMpBar->Draw();
	p_PlayUltGauge->Draw();

	DrawCrosshair();
	DrawControlGuide();
}

void UIManager::DrawCrosshair()
{
	// 画面中央座標
	int centerX = static_cast<int>(GameDefine::SCREEN_CENTER_X);
	int centerY = static_cast<int>(GameDefine::SCREEN_CENTER_Y);

	// 敵にヒットした直後は赤、それ以外は通常色で表示する
	int color = (_crosshairRedTimer > 0) ? CROSSHAIR_HIT_COLOR : CROSSHAIR_COLOR;

	// 上下左右4本の線を中心から隙間を空けて描画する
	DrawLine(centerX - GameDefine::CROSSHAIR_GAP - CROSSHAIR_ARM_LENGTH, centerY, centerX - GameDefine::CROSSHAIR_GAP, centerY, color, CROSSHAIR_THICKNESS);
	DrawLine(centerX + GameDefine::CROSSHAIR_GAP, centerY, centerX + GameDefine::CROSSHAIR_GAP + CROSSHAIR_ARM_LENGTH, centerY, color, CROSSHAIR_THICKNESS);
	DrawLine(centerX, centerY - GameDefine::CROSSHAIR_GAP - CROSSHAIR_ARM_LENGTH, centerX, centerY - GameDefine::CROSSHAIR_GAP, color, CROSSHAIR_THICKNESS);
	DrawLine(centerX, centerY + GameDefine::CROSSHAIR_GAP, centerX, centerY + GameDefine::CROSSHAIR_GAP + CROSSHAIR_ARM_LENGTH, color, CROSSHAIR_THICKNESS);
}

void UIManager::DrawControlGuide()
{
	// 半透明の紫背景
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, GUIDE_BOX_ALPHA);
	DrawBox(GUIDE_BOX_X, GUIDE_BOX_Y, GUIDE_BOX_X + GUIDE_BOX_WIDTH, GUIDE_BOX_Y + GUIDE_BOX_HEIGHT, GUIDE_BOX_COLOR, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	SetFontSize(GUIDE_FONT_SIZE);

	DrawExtendGraph(GUIDE_ICON_X, GUIDE_ROW1_Y, GUIDE_ICON_X + GUIDE_ICON_SIZE, GUIDE_ROW1_Y + GUIDE_ICON_SIZE, _guideYButtonH, true);
	DrawFormatString(GUIDE_TEXT_X, GUIDE_ROW1_Y + GUIDE_TEXT_Y_OFFSET, GUIDE_TEXT_COLOR, "Dodge");

	DrawExtendGraph(GUIDE_ICON_X, GUIDE_ROW2_Y, GUIDE_ICON_X + GUIDE_ICON_SIZE, GUIDE_ROW2_Y + GUIDE_ICON_SIZE, _guideBButtonH, true);
	DrawFormatString(GUIDE_TEXT_X, GUIDE_ROW2_Y + GUIDE_TEXT_Y_OFFSET, GUIDE_TEXT_COLOR, "Attack : Shot / Missile");

	DrawExtendGraph(GUIDE_ICON_X, GUIDE_ROW3_Y, GUIDE_ICON_X + GUIDE_ICON_SIZE, GUIDE_ROW3_Y + GUIDE_ICON_SIZE, _guideAButtonH, true);
	DrawFormatString(GUIDE_TEXT_X, GUIDE_ROW3_Y + GUIDE_TEXT_Y_OFFSET, GUIDE_TEXT_COLOR, "SP : Beam / Sword");

	SetFontSize(NORMAL_FONT_SIZE);
}
