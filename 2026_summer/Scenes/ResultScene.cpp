#include "ResultScene.h"
#include "Inputs/Input.h"
#include "GameDefine.h"
#include <DxLib.h>
#include <cstdio>
#include <cstring>

namespace
{
	// スカイドームの拡大率
	constexpr float DOME_SCALE = 3.0f;
	// ドームの回転速度
	constexpr float DOME_ROTATE_SPEED = 0.01f;
	// 「Press [Xボタン] to Restart...」の点滅周期
	constexpr int BLINK_CYCLE_FRAME = 60;
	// 点滅表示を行うフレーム数
	constexpr int BLINK_ON_FRAME = 30;
	// タイトル文字のフォントサイズ(セーフゾーンに収まるよう調整される前の基準サイズ)
	constexpr int TITLE_FONT_SIZE = 190;
	// スコア文字のフォントサイズ(同上)
	constexpr int SCORE_FONT_SIZE = 120;
	// 案内文字のフォントサイズ(同上)
	constexpr int PROMPT_FONT_SIZE = 100;
	// デフォルトのフォントサイズ
	constexpr int NORMAL_FONT_SIZE = 20;
	// タイトル文字の色
	constexpr int TITLE_COLOR = 0xffff00;
	// 縁取り文字の色
	constexpr int SHADOW_COLOR = 0x000000;
	// スコア文字の色
	constexpr int SCORE_COLOR = 0xffffff;
	// 案内文字の色
	constexpr int PROMPT_COLOR = 0xff00ff;
	// 縁取りのずらし量
	constexpr int SHADOW_OFFSET = 5;
	// タイトル・スコア文字のY座標
	constexpr int TITLE_Y = 100;
	constexpr int SCORE_Y = 450;
	// Xボタンアイコンの表示サイズ(セーフゾーンに収まるよう調整される前の基準サイズ)
	constexpr int XBUTTON_ICON_SIZE_BASE = PROMPT_FONT_SIZE + 30;
	// 文字とXボタンアイコンの間隔(同上)
	constexpr int XBUTTON_ICON_MARGIN_BASE = 15;
	// Xボタンアイコンを左上へ寄せるずらし量
	constexpr int XBUTTON_ICON_X_OFFSET = -10;
	constexpr int XBUTTON_ICON_Y_OFFSET = -15;
	// セーフゾーン(外周5%)の左右・下端
	constexpr int SAFE_LEFT_X = GameDefine::SAFE_ZONE_MARGIN_X;
	constexpr int SAFE_RIGHT_X = GameDefine::WIDTH - GameDefine::SAFE_ZONE_MARGIN_X;
	constexpr int SAFE_BOTTOM_Y = GameDefine::HEIGHT - GameDefine::SAFE_ZONE_MARGIN_Y;
	// セーフゾーンに収める際に使える横幅
	constexpr int SAFE_WIDTH = SAFE_RIGHT_X - SAFE_LEFT_X;
	// サイズ調整を打ち切る最小フォントサイズ(無限ループ防止)
	constexpr int MIN_FONT_SIZE = 20;

	// 指定した文字列の描画幅がtargetWidth以下になるまでフォントサイズを縮小する(最大5回)
	int FitFontSizeToWidth(const char* text, int textLen, int baseFontSize, int targetWidth, int& outWidth)
	{
		int fontSize = baseFontSize;
		for (int i = 0; i < 5; i++)
		{
			SetFontSize(fontSize);
			outWidth = GetDrawStringWidth(text, textLen);

			if (outWidth <= targetWidth || fontSize <= MIN_FONT_SIZE) break;

			float scale = static_cast<float>(targetWidth) / static_cast<float>(outWidth);
			fontSize = static_cast<int>(fontSize * scale);
			if (fontSize < MIN_FONT_SIZE) fontSize = MIN_FONT_SIZE;
		}
		return fontSize;
	}
}

ResultScene::ResultScene() :
	_canSceneChange(false),
	_xButtonH(-1),
	_domeH(-1), _count(0.0f),
	_uiCount(0),
	_titleFontSize(TITLE_FONT_SIZE), _titleX(SAFE_LEFT_X),
	_scoreFontSize(SCORE_FONT_SIZE), _scoreX(SAFE_LEFT_X),
	_promptFontSize(PROMPT_FONT_SIZE), _promptX(SAFE_LEFT_X), _promptY(SAFE_BOTTOM_Y - PROMPT_FONT_SIZE),
	_iconSize(XBUTTON_ICON_SIZE_BASE), _iconMargin(XBUTTON_ICON_MARGIN_BASE), _iconY(0)
{
}

ResultScene::~ResultScene()
{
}

void ResultScene::Init(int score, int handle)
{
	_score = score;
	_domeH = handle;
	_count = 0.0f;
	_canSceneChange = false;

	_xButtonH = LoadGraph("data/GUI/xbox_X.png");

	MV1SetPosition(_domeH, VGet(0.0f, 0.0f, 0.0f));
	MV1SetScale(_domeH, VGet(DOME_SCALE, DOME_SCALE, DOME_SCALE));

	// 外周5%セーフゾーンに収まるよう、実際の描画幅を測りながら各文字のサイズ・位置を調整する
	const char* titleText = "Game Result";
	int titleWidth = 0;
	_titleFontSize = FitFontSizeToWidth(titleText, static_cast<int>(strlen(titleText)), TITLE_FONT_SIZE, SAFE_WIDTH, titleWidth);
	_titleX = SAFE_LEFT_X + (SAFE_WIDTH - titleWidth) / 2;

	char scoreText[64];
	sprintf_s(scoreText, "KILL SCORE : %d!!!", _score);
	int scoreWidth = 0;
	_scoreFontSize = FitFontSizeToWidth(scoreText, static_cast<int>(strlen(scoreText)), SCORE_FONT_SIZE, SAFE_WIDTH, scoreWidth);
	_scoreX = SAFE_LEFT_X + (SAFE_WIDTH - scoreWidth) / 2;

	// 「Press 」[Xボタン]「to Restart...」をまとめてセーフゾーンに収める
	_promptFontSize = PROMPT_FONT_SIZE;
	_iconSize = XBUTTON_ICON_SIZE_BASE;
	_iconMargin = XBUTTON_ICON_MARGIN_BASE;

	int promptTotalWidth = 0;
	for (int i = 0; i < 5; i++)
	{
		SetFontSize(_promptFontSize);
		int prefixWidth = GetDrawStringWidth("Press ", static_cast<int>(sizeof("Press ") - 1));
		int suffixWidth = GetDrawStringWidth("to Restart...", static_cast<int>(sizeof("to Restart...") - 1));
		promptTotalWidth = prefixWidth + _iconMargin + _iconSize + _iconMargin + suffixWidth;

		if (promptTotalWidth <= SAFE_WIDTH || _promptFontSize <= MIN_FONT_SIZE) break;

		float scale = static_cast<float>(SAFE_WIDTH) / static_cast<float>(promptTotalWidth);
		_promptFontSize = static_cast<int>(_promptFontSize * scale);
		if (_promptFontSize < MIN_FONT_SIZE) _promptFontSize = MIN_FONT_SIZE;

		float sizeRate = static_cast<float>(_promptFontSize) / static_cast<float>(PROMPT_FONT_SIZE);
		_iconSize = static_cast<int>(XBUTTON_ICON_SIZE_BASE * sizeRate);
		_iconMargin = static_cast<int>(XBUTTON_ICON_MARGIN_BASE * sizeRate);
	}

	_promptX = SAFE_LEFT_X + (SAFE_WIDTH - promptTotalWidth) / 2;

	// 縦方向もセーフゾーン下端に収まるよう、文字とアイコンの高い方を基準に配置する
	int contentHeight = (_promptFontSize > _iconSize) ? _promptFontSize : _iconSize;
	_promptY = SAFE_BOTTOM_Y - contentHeight;
	_iconY = _promptY + XBUTTON_ICON_Y_OFFSET;

	SetFontSize(NORMAL_FONT_SIZE);
}

void ResultScene::End()
{
}

void ResultScene::Update(const std::shared_ptr<Input>& pInput)
{
	_count += DOME_ROTATE_SPEED;
	_uiCount++;

	MV1SetRotationXYZ(_domeH, VGet(0.0f, _count, 0.0f));

	if (pInput->IsTrigger(PAD_INPUT_1))
	{
		_canSceneChange = true;
	}

}

void ResultScene::Draw()
{
	MV1DrawModel(_domeH);

	SetFontSize(_titleFontSize);
	DrawFormatString(_titleX, TITLE_Y + SHADOW_OFFSET, TITLE_COLOR, "Game Result");
	DrawFormatString(_titleX, TITLE_Y, SHADOW_COLOR, "Game Result");

	SetFontSize(_scoreFontSize);
	DrawFormatString(_scoreX + SHADOW_OFFSET, SCORE_Y + SHADOW_OFFSET, SCORE_COLOR, "KILL SCORE : %d!!!", _score);
	DrawFormatString(_scoreX, SCORE_Y, SHADOW_COLOR, "KILL SCORE : %d!!!", _score);

	if (_uiCount % BLINK_CYCLE_FRAME <= BLINK_ON_FRAME)
	{
		SetFontSize(_promptFontSize);

		// 「X」の文字部分を実際のXボタンアイコンに差し替えて描画する
		int prefixWidth = GetDrawStringWidth("Press ", static_cast<int>(sizeof("Press ") - 1));
		int iconX = _promptX + prefixWidth + _iconMargin;
		int suffixX = iconX + _iconSize + _iconMargin;

		DrawFormatString(_promptX, _promptY, PROMPT_COLOR, "Press ");
		DrawFormatString(_promptX - SHADOW_OFFSET, _promptY - SHADOW_OFFSET, SHADOW_COLOR, "Press ");

		DrawExtendGraph(iconX + XBUTTON_ICON_X_OFFSET, _iconY,
			iconX + _iconSize + XBUTTON_ICON_X_OFFSET, _iconY + _iconSize, _xButtonH, true);

		DrawFormatString(suffixX, _promptY, PROMPT_COLOR, "to Restart...");
		DrawFormatString(suffixX - SHADOW_OFFSET, _promptY - SHADOW_OFFSET, SHADOW_COLOR, "to Restart...");
	}
	SetFontSize(NORMAL_FONT_SIZE);
}
