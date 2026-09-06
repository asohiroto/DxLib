#include "ExplainScene.h"
#include "Inputs/Input.h"
#include <DxLib.h>

namespace
{
	// 操作説明を読み飛ばせるようになるまでのフレーム数
	constexpr int SKIP_WAIT_FRAME = 60;
	// 「Press [Xボタン] to Game Start...」の点滅周期
	constexpr int BLINK_CYCLE_FRAME = 40;
	// 点滅表示を行うフレーム数
	constexpr int BLINK_ON_FRAME = 20;
	// 案内文字のフォントサイズ
	constexpr int PROMPT_FONT_SIZE = 60;
	// デフォルトのフォントサイズ
	constexpr int NORMAL_FONT_SIZE = 20;
	// 案内文字の色
	constexpr int PROMPT_COLOR = 0xffffff;
	// 縁取り文字の色
	constexpr int SHADOW_COLOR = 0x000000;
	// 縁取りのずらし量
	constexpr int SHADOW_OFFSET = 5;
	// 案内文字の描画位置
	constexpr int PROMPT_X = 355;
	constexpr int PROMPT_Y = 425;
	// Xボタンアイコンの表示サイズ
	constexpr int XBUTTON_ICON_SIZE = PROMPT_FONT_SIZE + 30;
	// 文字とXボタンアイコンの間隔
	constexpr int XBUTTON_ICON_MARGIN = 10;
	// Xボタンアイコンを左上へ寄せるずらし量
	constexpr int XBUTTON_ICON_X_OFFSET = -10;
	constexpr int XBUTTON_ICON_Y_OFFSET = -15;
}

ExplainScene::ExplainScene() :
	_explainH(-1),
	_xButtonH(-1),
	_canSceneChange(false),
	_count(0)
{
}

ExplainScene::~ExplainScene()
{
}

void ExplainScene::Init()
{
	_explainH = LoadGraph("data/「魔術師流自分の殺し方」操作説明.png");
	_xButtonH = LoadGraph("data/GUI/xbox_X.png");
	_count = 0;
}

void ExplainScene::End()
{
}

void ExplainScene::Update(const std::shared_ptr<Input>& pInput)
{
	_count++;

	if (_count > SKIP_WAIT_FRAME)
	{
		if (pInput->IsTrigger(PAD_INPUT_1))
		{
			_canSceneChange = true;
		}
	}
}

void ExplainScene::Draw()
{
	DrawGraph(0, 0, _explainH, false);

	if (_count % BLINK_CYCLE_FRAME < BLINK_ON_FRAME)
	{
		SetFontSize(PROMPT_FONT_SIZE);

		// 「X」の文字部分を実際のXボタンアイコンに差し替えて描画する
		int prefixWidth = GetDrawStringWidth("Press ", static_cast<int>(sizeof("Press ") - 1));
		int iconX = PROMPT_X + prefixWidth + XBUTTON_ICON_MARGIN;
		int suffixX = iconX + XBUTTON_ICON_SIZE + XBUTTON_ICON_MARGIN;

		DrawFormatString(PROMPT_X, PROMPT_Y, PROMPT_COLOR, "Press ");
		DrawFormatString(PROMPT_X - SHADOW_OFFSET, PROMPT_Y - SHADOW_OFFSET, SHADOW_COLOR, "Press ");

		DrawExtendGraph(iconX + XBUTTON_ICON_X_OFFSET, PROMPT_Y + XBUTTON_ICON_Y_OFFSET,
			iconX + XBUTTON_ICON_SIZE + XBUTTON_ICON_X_OFFSET, PROMPT_Y + XBUTTON_ICON_SIZE + XBUTTON_ICON_Y_OFFSET, _xButtonH, true);

		DrawFormatString(suffixX, PROMPT_Y, PROMPT_COLOR, "to Game Start...");
		DrawFormatString(suffixX - SHADOW_OFFSET, PROMPT_Y - SHADOW_OFFSET, SHADOW_COLOR, "to Game Start...");

		SetFontSize(NORMAL_FONT_SIZE);
	}
}
