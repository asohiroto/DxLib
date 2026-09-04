#include "ResultScene.h"
#include "Inputs/Input.h"
#include <DxLib.h>

namespace
{
	// スカイドームの拡大率
	constexpr float DOME_SCALE = 3.0f;
	// ドームの回転速度
	constexpr float DOME_ROTATE_SPEED = 0.01f;
	// 「Press X to Restart...」の点滅周期
	constexpr int BLINK_CYCLE_FRAME = 60;
	// 点滅表示を行うフレーム数
	constexpr int BLINK_ON_FRAME = 30;
	// タイトル文字のフォントサイズ
	constexpr int TITLE_FONT_SIZE = 190;
	// スコア文字のフォントサイズ
	constexpr int SCORE_FONT_SIZE = 120;
	// 案内文字のフォントサイズ
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
}

ResultScene::ResultScene() :
	_canSceneChange(false),
	_domeH(-1), _count(0.0f),
	_uiCount(0)
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

	MV1SetPosition(_domeH, VGet(0.0f, 0.0f, 0.0f));
	MV1SetScale(_domeH, VGet(DOME_SCALE, DOME_SCALE, DOME_SCALE));
}

void ResultScene::End()
{
}

void ResultScene::Update(std::shared_ptr<Input> pInput)
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

	SetFontSize(TITLE_FONT_SIZE);
	DrawFormatString(0, 110, TITLE_COLOR, "Game Result");
	DrawFormatString(0, 100, SHADOW_COLOR, "Game Result");
	SetFontSize(SCORE_FONT_SIZE);
	DrawFormatString(155, 455, SCORE_COLOR, "KILL SCORE : %d!!!", _score);
	DrawFormatString(150, 450, SHADOW_COLOR, "KILL SCORE : %d!!!", _score);
	if (_uiCount % BLINK_CYCLE_FRAME <= BLINK_ON_FRAME)
	{
		SetFontSize(PROMPT_FONT_SIZE);
		DrawFormatString(105, 805, PROMPT_COLOR, "Press X to Restart...");
		DrawFormatString(100, 800, SHADOW_COLOR, "Press X to Restart...");
	}
	SetFontSize(NORMAL_FONT_SIZE);
}
