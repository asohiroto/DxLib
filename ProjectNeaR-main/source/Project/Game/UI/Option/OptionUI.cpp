#include "OptionUI.h"
#include "../../../General/AssetManager.h"
#include "../../../General/Game.h"
#include "../../../General/Sound/SoundManager.h"
#include "../../../Main/Application.h"
#include <string>
#include <DxLib.h>

namespace
{
	//パス
	const std::wstring kBGMPath = L"Option/Option_BGM";
	const std::wstring kSEPath = L"Option/Option_SE";
	const std::wstring kVoicePath = L"Option/Option_Voice";
	const std::wstring kVolumeMarkPath = L"Option/VolumeMark";
	const std::wstring kScreenModePath = L"Option/Option_ScreenMode";
	const std::wstring kWindowModePath = L"Option/Option_Window";
	const std::wstring kFullScreenModePath = L"Option/Option_FullScreen";
	const std::wstring kGameBackPath = L"Option/Option_Back";
	const std::wstring kCursorPath = L"Mark/Cursor";
	const std::wstring kBackPath = L"Back/BlackBack1";

	//メニュー座標
	constexpr float kMenuPosX = Game::kScreenCenterX - 50.0f;
	constexpr float kMenuBasePosY = 100.0f;
	constexpr float kMenuOffsetPosY = 100.0f;
	//影オフセット
	constexpr float kShadowOffsetX = 10.0f;
	constexpr float kShadowOffsetY = 10.0f;
	//選んでるメニューをずらす
	constexpr float kSelectMenuOffsetX = 50.0f;
	//Lerpの速度
	constexpr float kLerpSpeedX = 0.1f;
	constexpr float kLerpSpeedY = 0.5f;

	
	//現在の音量のマークサイズ
	constexpr float kVolumeMarkSize = 1.1f;
	//音量マークの間隔
	constexpr float kVolumeMarkOffsetX = 10.0f;
	//音量数値のオフセット
	constexpr float kVolumeNumOffsetX = 150.0f;
	constexpr float kVolumeNumOffsetY = 16.0f;

	//スクリーンモード切り替え時のオフセット
	constexpr float kScreenModeOffsetX = 50.0f;
	constexpr float kScreenModeWindowOffsetY = 70.0f;
	constexpr float kScreenModeFullScreenOffsetY = kScreenModeWindowOffsetY * 2.0f;

	//カーソル座標
	constexpr float kBaseCursorPosX = kMenuPosX - 250.0f;
	constexpr float kScreenModeCursorPosX = kMenuPosX + kScreenModeOffsetX - 250.0f;


}

OptionUI::OptionUI() :
	UIBase(),
	m_bgmHandle(-1),
	m_seHandle(-1),
	m_voiceHandle(-1),
	m_volumeHandle(-1),
	m_screenModeHandle(-1),
	m_windowHandle(-1),
	m_fullScreenHandle(-1),
	m_gameBackHandle(-1),
	m_cursorHandle(-1),
	m_backHandle(-1),
	m_fontHandle(-1),
	m_cursorPos(),
	m_selectMenuIndex(OptionScene::OptionMenu::BGM),
	m_bgmVolume(1),
	m_seVolume(2),
	m_voiceVolume(3),
	m_isScreenMode(false),
	m_isFullScreen(false)
{
	auto& assetManager = AssetManager::GetInstance();
	//画像ハンドル取得
	m_bgmHandle = assetManager.GetImageHandle(kBGMPath);
	m_seHandle = assetManager.GetImageHandle(kSEPath);
	m_voiceHandle = assetManager.GetImageHandle(kVoicePath);
	m_volumeHandle = assetManager.GetImageHandle(kVolumeMarkPath);
	m_screenModeHandle = assetManager.GetImageHandle(kScreenModePath);
	m_windowHandle = assetManager.GetImageHandle(kWindowModePath);
	m_fullScreenHandle = assetManager.GetImageHandle(kFullScreenModePath);
	m_gameBackHandle = assetManager.GetImageHandle(kGameBackPath);
	m_cursorHandle = assetManager.GetImageHandle(kCursorPath);
	m_backHandle = assetManager.GetImageHandle(kBackPath);
	//フォントハンドル取得
	m_fontHandle = assetManager.GetFontHandle(AssetManager::Font(AssetManager::FontType::Roboto, AssetManager::FontSize::Size32));

	//選択中メニュー初期化
	m_selectMenuIndex = OptionScene::OptionMenu::BGM;

	//メニュー座標初期化
	m_menuPos.resize(static_cast<int>(OptionScene::OptionMenu::Max));
	for(int i = 0; i < static_cast<int>(OptionScene::OptionMenu::Max); i++)
	{
		m_menuPos[i] = { kMenuPosX , kMenuBasePosY + i * kMenuOffsetPosY };
	}
	m_cursorPos.x = kBaseCursorPosX;
	m_cursorPos.y = m_menuPos[0].y;

	//ウィンドウモード
	m_isFullScreen = !Application::GetInstance().IsWindowMode();
}

OptionUI::~OptionUI()
{
}

void OptionUI::Update()
{
	//選択中のメニューをスライド
	for(int i = 0; i < m_menuPos.size(); i++)
	{
		//選択中
		if (i == static_cast<int>(m_selectMenuIndex))
		{
			if (!m_isScreenMode)
			{
				m_cursorPos.y = MathSub::Lerp(m_cursorPos.y, m_menuPos[i].y, kLerpSpeedY);
				m_cursorPos.x = MathSub::Lerp(m_cursorPos.x,kBaseCursorPosX, kLerpSpeedX);
			}
			//選択中メニュー
			m_menuPos[i].x = MathSub::Lerp(m_menuPos[i].x,kMenuPosX + kSelectMenuOffsetX, kLerpSpeedX);
		}
		else
		{
			//選択中でないメニュー
			m_menuPos[i].x = MathSub::Lerp(m_menuPos[i].x, kMenuPosX, kLerpSpeedX);
		}
	}

	//スクリーンモード切り替え中
	if (m_isScreenMode)
	{
		if (m_isFullScreen)
		{
			m_cursorPos.y = MathSub::Lerp(m_cursorPos.y, m_menuPos[static_cast<int>(OptionScene::OptionMenu::ScreenMode)].y + kScreenModeFullScreenOffsetY, kLerpSpeedY);
		}
		else
		{
			m_cursorPos.y = MathSub::Lerp(m_cursorPos.y, m_menuPos[static_cast<int>(OptionScene::OptionMenu::ScreenMode)].y + kScreenModeWindowOffsetY, kLerpSpeedY);
		}
		m_cursorPos.x = MathSub::Lerp(m_cursorPos.x, kScreenModeCursorPosX, kLerpSpeedX);
	}

	//各音量取得
	auto& soundManager = SoundManager::GetInstance();

	m_bgmVolume = soundManager.GetLevelToVolume(soundManager.GetBGMVolume());
	m_seVolume = soundManager.GetLevelToVolume(soundManager.GetSEVolume());
	m_voiceVolume = soundManager.GetLevelToVolume(soundManager.GetVoiceVolume());
}

void OptionUI::Draw() const
{
	//背景
	DrawGraph(0, 0, m_backHandle, true);

	int bgmIndex = static_cast<int>(OptionScene::OptionMenu::BGM);
	int seIndex = static_cast<int>(OptionScene::OptionMenu::SE);
	int voiceIndex = static_cast<int>(OptionScene::OptionMenu::Voice);
	int screenModeIndex = static_cast<int>(OptionScene::OptionMenu::ScreenMode);
	int gameBackIndex = static_cast<int>(OptionScene::OptionMenu::Back);

	//影
	SetDrawBlendMode(DX_BLENDMODE_SUB, 100);
	DrawRotaGraphF(m_menuPos[bgmIndex].x + kShadowOffsetX, m_menuPos[bgmIndex].y + kShadowOffsetY, 1.0f, 0.0f, m_bgmHandle, true);
	DrawRotaGraphF(m_menuPos[seIndex].x + kShadowOffsetX, m_menuPos[seIndex].y + kShadowOffsetY, 1.0f, 0.0f, m_seHandle, true);
	DrawRotaGraphF(m_menuPos[voiceIndex].x + kShadowOffsetX, m_menuPos[voiceIndex].y + kShadowOffsetY, 1.0f, 0.0f, m_voiceHandle, true);
	DrawRotaGraphF(m_menuPos[screenModeIndex].x + kShadowOffsetX, m_menuPos[screenModeIndex].y + kShadowOffsetY, 1.0f, 0.0f, m_screenModeHandle, true);
	DrawRotaGraphF(m_menuPos[gameBackIndex].x + kShadowOffsetX, m_menuPos[gameBackIndex].y + kShadowOffsetY, 1.0f, 0.0f, m_gameBackHandle, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);

	//描画
	DrawRotaGraphF(m_menuPos[bgmIndex].x, m_menuPos[bgmIndex].y, 1.0f, 0.0f, m_bgmHandle, true);
	DrawRotaGraphF(m_menuPos[seIndex].x, m_menuPos[seIndex].y, 1.0f, 0.0f, m_seHandle, true);
	DrawRotaGraphF(m_menuPos[voiceIndex].x, m_menuPos[voiceIndex].y, 1.0f, 0.0f, m_voiceHandle, true);
	DrawRotaGraphF(m_menuPos[screenModeIndex].x, m_menuPos[screenModeIndex].y, 1.0f, 0.0f, m_screenModeHandle, true);
	DrawRotaGraphF(m_menuPos[gameBackIndex].x, m_menuPos[gameBackIndex].y, 1.0f, 0.0f, m_gameBackHandle, true);

	//ボリューム数値描画
	DrawFormatStringToHandle(m_menuPos[bgmIndex].x + kVolumeNumOffsetX, m_menuPos[bgmIndex].y - kVolumeNumOffsetY, 0xffffff, m_fontHandle, L"%d", m_bgmVolume);
	DrawFormatStringToHandle(m_menuPos[seIndex].x + kVolumeNumOffsetX, m_menuPos[seIndex].y - kVolumeNumOffsetY, 0xffffff, m_fontHandle, L"%d", m_seVolume);
	DrawFormatStringToHandle(m_menuPos[voiceIndex].x + kVolumeNumOffsetX, m_menuPos[voiceIndex].y - kVolumeNumOffsetY, 0xffffff, m_fontHandle, L"%d", m_voiceVolume);

	SetDrawBlendMode(DX_BLENDMODE_INVSRC, 255);
	switch (static_cast<OptionScene::OptionMenu>(m_selectMenuIndex))
	{
	case OptionScene::OptionMenu::BGM:
		DrawRotaGraphF(m_menuPos[bgmIndex].x, m_menuPos[bgmIndex].y, 1.0f, 0.0f, m_bgmHandle, true);
		DrawFormatStringToHandle(m_menuPos[bgmIndex].x + kVolumeNumOffsetX, m_menuPos[bgmIndex].y - kVolumeNumOffsetY, 0xffffff, m_fontHandle, L"%d", m_bgmVolume);
		break;
	case OptionScene::OptionMenu::SE:
		DrawRotaGraphF(m_menuPos[seIndex].x, m_menuPos[seIndex].y, 1.0f, 0.0f, m_seHandle, true);
		DrawFormatStringToHandle(m_menuPos[seIndex].x + kVolumeNumOffsetX, m_menuPos[seIndex].y - kVolumeNumOffsetY, 0xffffff, m_fontHandle, L"%d", m_seVolume);
		break;
	case OptionScene::OptionMenu::Voice:
		DrawRotaGraphF(m_menuPos[voiceIndex].x, m_menuPos[voiceIndex].y, 1.0f, 0.0f, m_voiceHandle, true);
		DrawFormatStringToHandle(m_menuPos[voiceIndex].x + kVolumeNumOffsetX, m_menuPos[voiceIndex].y - kVolumeNumOffsetY, 0xffffff, m_fontHandle, L"%d", m_voiceVolume);
		break;
	case OptionScene::OptionMenu::ScreenMode:
		DrawRotaGraphF(m_menuPos[screenModeIndex].x, m_menuPos[screenModeIndex].y, 1.0f, 0.0f, m_screenModeHandle, true);
		break;
	case OptionScene::OptionMenu::Back:
		DrawRotaGraphF(m_menuPos[gameBackIndex].x, m_menuPos[gameBackIndex].y, 1.0f, 0.0f, m_gameBackHandle, true);
		break;
	default:
		break;
	}
	
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);

	//ボリュームマーク描画
	SetDrawBlendMode(DX_BLENDMODE_SUB, 255);
	for(int i = 0; i < 10; i++)
	{
		DrawRotaGraphF(m_menuPos[bgmIndex].x + i * kVolumeMarkOffsetX, m_menuPos[bgmIndex].y, 1.0f, 0.0f, m_volumeHandle, true);
		DrawRotaGraphF(m_menuPos[seIndex].x + i * kVolumeMarkOffsetX, m_menuPos[seIndex].y, 1.0f, 0.0f, m_volumeHandle, true);
		DrawRotaGraphF(m_menuPos[voiceIndex].x + i * kVolumeMarkOffsetX, m_menuPos[voiceIndex].y, 1.0f, 0.0f, m_volumeHandle, true);
	}
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);

	//各音量分描画
	DrawVolume(m_bgmVolume, bgmIndex);
	DrawVolume(m_seVolume, seIndex);
	DrawVolume(m_voiceVolume, voiceIndex);

	//スクリーンモード描画
	if (m_isScreenMode)
	{
		//背景を少し暗く
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
		DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);

		float windowPosX = m_menuPos[screenModeIndex].x + kScreenModeOffsetX;
		float windowPosY = m_menuPos[screenModeIndex].y + kScreenModeWindowOffsetY;
		float fullScreenPosX = m_menuPos[screenModeIndex].x + kScreenModeOffsetX;
		float fullScreenPosY = m_menuPos[screenModeIndex].y + kScreenModeFullScreenOffsetY;

		//影
		SetDrawBlendMode(DX_BLENDMODE_SUB, 100);
		DrawRotaGraphF(windowPosX + kShadowOffsetX, windowPosY + kShadowOffsetY, 1.0f, 0.0f, m_windowHandle, true);
		DrawRotaGraphF(fullScreenPosX + kShadowOffsetX, fullScreenPosY + kShadowOffsetY, 1.0f, 0.0f, m_fullScreenHandle, true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);

		//描画
		DrawRotaGraphF(windowPosX, windowPosY, 1.0f, 0.0f, m_windowHandle, true);
		DrawRotaGraphF(fullScreenPosX, fullScreenPosY, 1.0f, 0.0f, m_fullScreenHandle, true);

		//選んでるほうを反転
		SetDrawBlendMode(DX_BLENDMODE_INVSRC, 255);
		if (m_isFullScreen)
		{
			DrawRotaGraphF(fullScreenPosX, fullScreenPosY, 1.0f, 0.0f, m_fullScreenHandle, true);
		}
		else
		{
			DrawRotaGraphF(windowPosX, windowPosY, 1.0f, 0.0f, m_windowHandle, true);
		}

		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}

	//影
	SetDrawBlendMode(DX_BLENDMODE_SUB, 100);
	DrawRotaGraph(m_cursorPos.x + kShadowOffsetX, m_cursorPos.y + kShadowOffsetY, 1.0f, 0.0f, m_cursorHandle, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	SetDrawBlendMode(DX_BLENDMODE_INVSRC, 255);
	//カーソル描画
	DrawRotaGraphF(m_cursorPos.x, m_cursorPos.y, 1.0f, 0.0f, m_cursorHandle, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);

}

void OptionUI::SetSelectMenuIndex(OptionScene::OptionMenu index)
{
	m_selectMenuIndex = index;
}


void OptionUI::DrawVolume(int volume, int index) const
{
	for (int i = 0; i < volume; i++)
	{
		float size = 1.0f;
		if (i == volume - 1) size = kVolumeMarkSize;
		DrawRotaGraphF(m_menuPos[index].x + i * kVolumeMarkOffsetX, m_menuPos[index].y, size, 0.0f, m_volumeHandle, true);
	}
}
