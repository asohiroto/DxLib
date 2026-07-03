#include "DialogUI.h"
#include "../../../General/AssetManager.h"
#include "../../../General/Game.h"
#include "../../../General/StringUtil.h"
#include "../../../General/Input.h"
#include <DxLib.h>

namespace
{
	//パス
	const std::wstring kYesPath = L"Dialog/Yes";
	const std::wstring kNoPath = L"Dialog/No";
	const std::wstring kBackPath = L"Dialog/DialogBack";
	const std::wstring kCursorPath = L"Mark/Cursor_Mini";
	const std::wstring kBackShadowPath = L"Dialog/DialogBack_Shadow";

	//座標
	constexpr int kBackPosX = Game::kScreenCenterX;
	constexpr int kBackPosY = Game::kScreenCenterY;
	constexpr int kTextPosX = kBackPosX;
	constexpr int kTextPosY = kBackPosY - 50;
	constexpr int kYesPosX = kBackPosX - 140;
	constexpr int kNoPosX = kBackPosX + 140;
	constexpr int kReplyPosY = kBackPosY + 100;

	//影
	constexpr int kShadowOffsetPosX = 10;
	constexpr int kShadowOffsetPosY = 10;

	//カーソル
	constexpr int kCursorOffsetPosX = 130;
	constexpr int kCursorOutPosX = -500;
	constexpr float kCursorLerpRate = 0.3f;

	//1秒選べなくする
	constexpr int kStopInputFrame = 45;

}

DialogUI::DialogUI(std::wstring text):
	UIBase(),
	m_yesHandle(-1),
	m_noHandle(-1),
	m_backHandle(-1),
	m_cursorHandle(-1),
	m_textHandle(-1),
	m_backShadowHandle(-1),
	m_text(text),
	m_isYes(true),
	m_cursorPos(kYesPosX - kCursorOffsetPosX, kReplyPosY),
	m_countFrame(0)
{
	auto& assetManager = AssetManager::GetInstance();
	m_yesHandle = assetManager.GetImageHandle(kYesPath);
	m_noHandle = assetManager.GetImageHandle(kNoPath);
	m_backHandle = assetManager.GetImageHandle(kBackPath);
	m_cursorHandle = assetManager.GetImageHandle(kCursorPath);
	m_backShadowHandle = assetManager.GetImageHandle(kBackShadowPath);
	m_textHandle = assetManager.GetFontHandle(AssetManager::Font(AssetManager::FontType::NotoSansJP, AssetManager::FontSize::Size24));
}

DialogUI::~DialogUI()
{
}

void DialogUI::Update()
{
	if (!m_isDraw)
	{
		//描画してないときは「はい」で待機
		m_isYes = true;

		//カウントリセット
		m_countFrame = 0;

		//画面外で待機
		m_cursorPos = Vector2(kCursorOutPosX, kReplyPosY);
	}
	else
	{
		//数フレーム入力無効
		if (m_countFrame < kStopInputFrame)
		{
			++m_countFrame;
			//無効
			Input::GetInstance().StopUpdate();
		}
		else
		{
			//有効
			Input::GetInstance().StartUpdate();

			//選んでいるほうにlerp
			Vector2 selectPos = Vector2(kYesPosX - kCursorOffsetPosX, kReplyPosY);
			if (!m_isYes)selectPos = Vector2(kNoPosX - kCursorOffsetPosX, kReplyPosY);
			m_cursorPos = Vector2::Lerp(m_cursorPos, selectPos, kCursorLerpRate);
		}
	}
}

void DialogUI::Draw() const
{
	if (!m_isDraw)return;

	//影
	DrawRotaGraph(kBackPosX + kShadowOffsetPosX, kBackPosY + kShadowOffsetPosY, 1.0, 0.0, m_backShadowHandle, true);

	//背景
	DrawRotaGraph(kBackPosX, kBackPosY, 1.0, 0.0, m_backHandle, true);

	//テキスト(中央ぞろえ)
	float textWidth = StringUtil::GetTextWidth(m_text, m_textHandle);
	textWidth *= 0.5f;
	DrawStringToHandle(kTextPosX - textWidth, kTextPosY, m_text.c_str(), 0xffffff, m_textHandle);

	//通常描画
	DrawRotaGraph(kYesPosX, kReplyPosY, 1.0, 0.0, m_yesHandle, true);
	DrawRotaGraph(kNoPosX, kReplyPosY, 1.0, 0.0, m_noHandle, true);


	if (m_countFrame >= kStopInputFrame)
	{
		//選んでいるほうを反転描画
		SetDrawBlendMode(DX_BLENDMODE_INVSRC, 255);
		if (m_isYes)
		{
			DrawRotaGraph(kYesPosX, kReplyPosY, 1.0, 0.0, m_yesHandle, true);
		}
		else
		{
			DrawRotaGraph(kNoPosX, kReplyPosY, 1.0, 0.0, m_noHandle, true);
		}
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}

	//カーソル
	SetDrawBlendMode(DX_BLENDMODE_INVSRC, 255);
	DrawRotaGraphF(m_cursorPos.x, m_cursorPos.y, 1.0, 0.0, m_cursorHandle, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
}
