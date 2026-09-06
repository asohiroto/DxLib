#include "StartScene.h"
#include "Inputs/Input.h"
#include "SkyDome.h"
#include "GameDefine.h"
#include <DxLib.h>
#include <cmath>

namespace
{
	// ドームの回転速度
	constexpr float ROTATE_SPEED = 0.005f;
	// フォントサイズ(セーフゾーンに収まるよう調整される前の基準サイズ)
	constexpr int FONT_SIZE = 120;
	// デフォルトのフォントサイズ
	constexpr int NORMAL_FONT_SIZE = 20;
	// 案内文字の色
	constexpr int PROMPT_COLOR = 0xff00ff;
	// 縁取り文字の色
	constexpr int SHADOW_COLOR = 0x000000;
	// 上下に揺れるアニメーションの速さ
	constexpr float BOB_SPEED = 0.1f;
	// 「Press」の揺れ幅
	constexpr float BOB_AMPLITUDE_1 = 25.0f;
	// 「to Start...」の揺れ幅
	constexpr float BOB_AMPLITUDE_2 = 30.0f;
	// Xボタンアイコンの表示サイズ(セーフゾーンに収まるよう調整される前の基準サイズ)
	constexpr int XBUTTON_ICON_SIZE = 240;
	// 文字とXボタンアイコンの間隔(セーフゾーンに収まるよう調整される前の基準サイズ)
	constexpr int XBUTTON_ICON_MARGIN = 20;
	// 案内文字のY座標(前回より少し上へ)
	constexpr int PROMPT_TEXT_Y = 720;
	// 左右の揺れ幅の最大値(この分だけ内側に余裕を持たせてセーフゾーンからはみ出さないようにする)
	constexpr float MAX_BOB_AMPLITUDE = (BOB_AMPLITUDE_1 > BOB_AMPLITUDE_2) ? BOB_AMPLITUDE_1 : BOB_AMPLITUDE_2;
	// 案内文字列の左端の基準X座標(外周5%セーフゾーン+最大揺れ幅ぶん内側)
	constexpr int PROMPT_BASE_X = GameDefine::SAFE_ZONE_MARGIN_X + static_cast<int>(MAX_BOB_AMPLITUDE);
	// 案内文字列・アイコンが収まるべき右端のX座標(同じく揺れ幅ぶん内側)
	constexpr int PROMPT_SAFE_RIGHT_X = GameDefine::WIDTH - GameDefine::SAFE_ZONE_MARGIN_X - static_cast<int>(MAX_BOB_AMPLITUDE);
	// サイズ調整を打ち切る最小フォントサイズ(無限ループ防止)
	constexpr int MIN_FONT_SIZE = 20;
}

StartScene::StartScene() :
	p_Dome(nullptr),
	_logoH(-1),
	_angle(0.0f),
	_startY1(0), _startY2(0),
	_count(0),
	_xButtonH(-1),
	_fontSize(FONT_SIZE),
	_iconSize(XBUTTON_ICON_SIZE),
	_iconMargin(XBUTTON_ICON_MARGIN),
	_iconY(PROMPT_TEXT_Y)
{
}

StartScene::~StartScene()
{
}

void StartScene::Init(int domeH)
{
	p_Dome = std::make_shared<SkyDome>();
	p_Dome->Init(domeH);

	_logoH = LoadGraph("data/2026_summer_Logo.png");
	_xButtonH = LoadGraph("data/GUI/xbox_X.png");

	_isSceneChange = false;

	// 左右の揺れを含めても外周5%セーフゾーンに収まるよう、実際の描画幅を測りながら
	// 文字とアイコンの大きさを縮小していく(数回で収まる大きさに収束する)
	int availableWidth = PROMPT_SAFE_RIGHT_X - PROMPT_BASE_X;

	_fontSize = FONT_SIZE;
	_iconSize = XBUTTON_ICON_SIZE;
	_iconMargin = XBUTTON_ICON_MARGIN;

	for (int i = 0; i < 5; i++)
	{
		SetFontSize(_fontSize);
		int prefixWidth = GetDrawStringWidth("Press", static_cast<int>(sizeof("Press") - 1));
		int suffixWidth = GetDrawStringWidth("to Start...", static_cast<int>(sizeof("to Start...") - 1));
		int totalWidth = prefixWidth + _iconMargin + _iconSize + _iconMargin + suffixWidth;

		if (totalWidth <= availableWidth || _fontSize <= MIN_FONT_SIZE) break;

		float scale = static_cast<float>(availableWidth) / static_cast<float>(totalWidth);
		_fontSize = static_cast<int>(_fontSize * scale);
		if (_fontSize < MIN_FONT_SIZE) _fontSize = MIN_FONT_SIZE;

		float sizeRate = static_cast<float>(_fontSize) / static_cast<float>(FONT_SIZE);
		_iconSize = static_cast<int>(XBUTTON_ICON_SIZE * sizeRate);
		_iconMargin = static_cast<int>(XBUTTON_ICON_MARGIN * sizeRate);
	}

	// アイコンの縦中心を文字の縦中心に合わせる(文字の高さはおおよそフォントサイズに等しい)
	_iconY = PROMPT_TEXT_Y + _fontSize / 2 - _iconSize / 2;

	SetFontSize(NORMAL_FONT_SIZE);
}

void StartScene::End()
{
}

void StartScene::Update(const std::shared_ptr<Input>& pInput)
{
	_angle += ROTATE_SPEED;
	_count++;

	_startY1 = static_cast<int>(sin(_count * BOB_SPEED) * BOB_AMPLITUDE_1);
	_startY2 = static_cast<int>(sin(_count * BOB_SPEED) * BOB_AMPLITUDE_2);

	p_Dome->Update();
	p_Dome->SetRotate(_angle);

	if (pInput->IsTrigger(PAD_INPUT_1))
		_isSceneChange = true;
}

void StartScene::Draw()
{
	p_Dome->Draw();
	DrawGraph(200, 50, _logoH, true);

	SetFontSize(_fontSize);

	// アイコンサイズが変わっても重ならないよう、「Press」の描画幅から後続の位置を算出する
	int prefixWidth = GetDrawStringWidth("Press", static_cast<int>(sizeof("Press") - 1));
	int iconX = PROMPT_BASE_X + prefixWidth + _iconMargin;
	int suffixX = iconX + _iconSize + _iconMargin;

	DrawFormatString(PROMPT_BASE_X + _startY1, PROMPT_TEXT_Y, PROMPT_COLOR, "Press");
	DrawFormatString(PROMPT_BASE_X + _startY2, PROMPT_TEXT_Y, SHADOW_COLOR, "Press");
	DrawExtendGraph(iconX + _startY1, _iconY, iconX + _startY1 + _iconSize, _iconY + _iconSize, _xButtonH, true);
	DrawFormatString(suffixX + _startY1, PROMPT_TEXT_Y, PROMPT_COLOR, "to Start...");
	DrawFormatString(suffixX + _startY2, PROMPT_TEXT_Y, SHADOW_COLOR, "to Start...");
	SetFontSize(NORMAL_FONT_SIZE);
}
