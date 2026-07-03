#include "BossHPUI.h"
#include "../../../General/AssetManager.h"
#include "../../../General/CharaStatus.h"
#include "../../../General/Game.h"
#include "../../Actor/Character/Enemy/EnemyBase.h"
#include <DxLib.h>
#include <cassert>

namespace
{
	//バーの変動フレーム
	constexpr int kFluctuationValueFrame = 120;
	//バーの画像サイズ
	constexpr float kBarSizeX = 20.0f;
	constexpr float kBarSizeY = 300.0f;
	//体力に変動があった場合10秒間表示
	constexpr int kChangeHPFrame = 60 * 10;

	//基準位置
	constexpr float kPosX = Game::kScreenWidth - kBarSizeX - 70.0f;
	constexpr float kPosY = Game::kScreenCenterY + (kBarSizeY * 0.5f);

	//背景
	constexpr float kBackBarSizeX = 24.0f;
	constexpr float kBackBarSizeY = 306.0f;
	constexpr float kBackPosX = Game::kScreenWidth - kBackBarSizeX - 70.0f + 2.0f;
	constexpr float kBackPosY = Game::kScreenCenterY - kBackBarSizeY * 0.5f;

	//ボス名
	constexpr float kBossNamePosX = kPosX - 20.0f;
	constexpr float kBossNamePosY = Game::kScreenCenterY;

	//ハンドル
	const std::wstring kHPValue = L"Enemy/BossHPValue";
	const std::wstring kHPValueBack = L"Enemy/BossHPValueBack";
	const std::wstring kHPValueDamage = L"Enemy/BossHPValueDamage";
	const std::wstring kHPValueHeal = L"Enemy/BossHPValueHeal";
	const std::wstring kBossName = L"Enemy/Boss";
}

BossHPUI::BossHPUI(std::shared_ptr<CharaStatus> charaStatus, std::weak_ptr<EnemyBase> pEnemy) :
	HPUIBase(charaStatus),
	m_pEnemy(pEnemy)

{
	auto& assetManager = AssetManager::GetInstance();
	m_nowValueHandle = assetManager.GetImageHandle(kHPValue);
	m_backValueHandle = assetManager.GetImageHandle(kHPValueBack);
	m_damageValueHandle = assetManager.GetImageHandle(kHPValueDamage);
	m_healValueHandle = assetManager.GetImageHandle(kHPValueHeal);
	m_bossNameHandle = assetManager.GetImageHandle(kBossName);
	m_beforeNowHP = m_charaStatus->GetNowHP();
	m_nowHpRate = m_charaStatus->GetHPRate();
	m_damageValueRate = m_charaStatus->GetHPRate();
	m_healValueRate = m_charaStatus->GetHPRate();
}

BossHPUI::~BossHPUI()
{
}

void BossHPUI::Update()
{

	if (m_pEnemy.expired())
	{
		m_isDelete = true;
		return;
	}
	auto enemy = m_pEnemy.lock();
	if (!enemy->IsActive())return;

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

}

void BossHPUI::Draw() const
{
	if (m_pEnemy.expired())return;
	auto enemy = m_pEnemy.lock();
	if (!enemy->IsActive())return;
	//バーの描画
	DrawRectGraphF2(kBackPosX, kBackPosY, 0, 0, kBackBarSizeX, kBackBarSizeY, m_backValueHandle, true);
	//バーの描画
	DrawRectGraphF2(kPosX, kPosY, 0, 0, kBarSizeX , (kBarSizeY * -m_damageValueRate), m_damageValueHandle, true);
	DrawRectGraphF2(kPosX, kPosY, 0, 0, kBarSizeX , (kBarSizeY * -m_healValueRate), m_healValueHandle, true);
	DrawRectGraphF2(kPosX, kPosY, 0, 0, kBarSizeX , (kBarSizeY * -m_nowHpRate), m_nowValueHandle, true);
	//ボス名の描画
	DrawRotaGraphF(kBossNamePosX, kBossNamePosY, 1.0f, 0.0f, m_bossNameHandle, true);
}
