#include "SkipUI.h"
#include "Input/BottunUI.h"
#include "../../General/Game.h"
#include "../../General/Math/MyMath.h"
#include "../../General/AssetManager.h"
#include <DxLib.h>

namespace
{
	//パス
	const std::wstring kSkipUIPath = L"TestCircle";
	const std::wstring kABottunPath = L"A";

	//スキップ
	const std::wstring kSkipText = L"長押しでスキップ";

	//ボタン座標
	constexpr float kSkipPosX = Game::kScreenCenterX + 350.0f; 
	constexpr float kSkipPosY = Game::kScreenHeight - 50.0f; 
	//テキスト座標
	constexpr float kSkipTextPosX = kSkipPosX + 40.0f;
	constexpr float kSkipTextPosY = kSkipPosY - 10.0f;

	//透明度変化速度
	constexpr int kAlphaChangeSpeed = 2;
}

SkipUI::SkipUI():
	UIBase(),
	m_rate(0.0f),
	m_handle(-1),
	m_fontHandle(-1),
	m_alpha(0)
{
	auto& assetManager = AssetManager::GetInstance();

	//ハンドル取得
	m_handle = assetManager.GetImageHandle(kSkipUIPath);
	//ボタンUI
	auto bottunUI = std::make_shared<BottunUI>(kABottunPath);
	bottunUI->SetPos({ kSkipPosX , kSkipPosY});
	m_bottunUI = bottunUI;

	//フォントハンドル
	m_fontHandle = assetManager.GetFontHandle(AssetManager::Font(AssetManager::FontType::NotoSansJP, AssetManager::FontSize::Size20));
}

SkipUI::~SkipUI()
{
}

void SkipUI::Update()
{
	//透明に
	if (m_rate <= 0.0f)
	{
		//だんだん見えなくなる
		m_alpha -= kAlphaChangeSpeed;
		m_alpha = MathSub::ClampInt(m_alpha, 0, 255);
	}
	else
	{
		//見えるように
		m_alpha = 255;
	}
}

void SkipUI::Draw() const
{
	if (!m_isDraw)return;

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_alpha);

	//ゲージ
	DrawCircleGauge(kSkipPosX, kSkipPosY, m_rate, m_handle);

	//ボタンUI描画
	m_bottunUI->Draw();

	//スキップ
	DrawStringToHandle(kSkipTextPosX, kSkipTextPosY, kSkipText.c_str(), 0xffffff, m_fontHandle);
}

void SkipUI::SetRate(float rate)
{
	if(m_rate >= 100.0f)
	{
		return;
	}
	else
	{
		m_rate = rate * 100.0f;
	}

	m_rate = MathSub::ClampFloat(m_rate, 0.0f, 100.0f);
}
