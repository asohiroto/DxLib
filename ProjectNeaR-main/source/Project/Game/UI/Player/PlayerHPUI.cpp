#include "PlayerHPUI.h"
#include "../../../General/AssetManager.h"
#include "../../../General/CharaStatus.h"
#include "../../../General/Math/MyMath.h"
#include <DxLib.h>

namespace
{
	//バーの変動フレーム
	constexpr int kFluctuationValueFrame = 120;
	//基準位置
	constexpr float kBasePosX = 100.0f;
	constexpr float kBasePosY = 70.0f;
	//バーの画像サイズ
	constexpr float kBarSizeX = 250.0f;
	constexpr float kBarSizeY = 20.0f;

	//ハンドル
	const std::wstring kHPValue = L"Player/HPValue";
	const std::wstring kHPValueBack = L"Player/HPValueBack";
	const std::wstring kHPValueDamage = L"Player/HPValueDamage";
	const std::wstring kHPValueHeal = L"Player/HPValueHeal";
	const std::wstring kHPBarFrame = L"Player/HPFrame";

	//Lerpの割合
	constexpr float kLerpRate = 0.1f;
}

PlayerHPUI::PlayerHPUI(std::shared_ptr<CharaStatus> charaStatus):
	HPUIBase(charaStatus),
	m_pos{-kBasePosX, kBasePosY}
{
	auto& assetManager = AssetManager::GetInstance();
	m_nowValueHandle = assetManager.GetImageHandle(kHPValue);
	m_backValueHandle = assetManager.GetImageHandle(kHPValueBack);
	m_damageValueHandle = assetManager.GetImageHandle(kHPValueDamage);
	m_healValueHandle = assetManager.GetImageHandle(kHPValueHeal);
	m_barFrameHandle = assetManager.GetImageHandle(kHPBarFrame);
	m_beforeNowHP = m_charaStatus->GetNowHP();
	m_nowHpRate = m_charaStatus->GetHPRate();
	m_damageValueRate = m_charaStatus->GetHPRate();
	m_healValueRate = m_charaStatus->GetHPRate();
}

PlayerHPUI::~PlayerHPUI()
{
}

void PlayerHPUI::Update()
{
	float nowHP = m_charaStatus->GetNowHP();
	//体力が前のフレームより低かったら
	if (nowHP < m_beforeNowHP)
	{
		//ダメージを受けた
		m_damageFrame = kFluctuationValueFrame;
		//回復バーは体力に合わせる
		m_healValueRate = m_charaStatus->GetHPRate();
	}
	else if (nowHP > m_beforeNowHP)
	{
		//回復
		m_healFrame = kFluctuationValueFrame;
		//回復バーは体力に合わせる
		m_healValueRate = m_charaStatus->GetHPRate();
	}
	//現在の体力を更新
	m_beforeNowHP = nowHP;

	//ダメージを受けた
	UpdateDamageBar();

	//体力が回復
	UpdateHealBar();

	//描画位置への移動
	m_pos = Vector2::Lerp(m_pos, Vector2(kBasePosX, kBasePosY), kLerpRate);
}

void PlayerHPUI::Draw() const
{
	if (!m_isDraw)return;
	//周りのフレーム
	DrawRotaGraphF((m_pos.x + m_pos.x + kBarSizeX) * 0.5f, (m_pos.y + m_pos.y + kBarSizeY) * 0.5f, 1.0, 0.0, m_barFrameHandle, true);
	//バーの描画
	DrawExtendGraphF(m_pos.x, m_pos.y, m_pos.x + kBarSizeX, m_pos.y + kBarSizeY, m_backValueHandle, true);
	DrawExtendGraphF(m_pos.x, m_pos.y, m_pos.x + (kBarSizeX * m_damageValueRate), m_pos.y + kBarSizeY, m_damageValueHandle, true);
	DrawExtendGraphF(m_pos.x, m_pos.y, m_pos.x + (kBarSizeX * m_healValueRate), m_pos.y + kBarSizeY, m_healValueHandle, true);
	DrawExtendGraphF(m_pos.x, m_pos.y, m_pos.x + (kBarSizeX * m_nowHpRate), m_pos.y + kBarSizeY, m_nowValueHandle, true);
}
