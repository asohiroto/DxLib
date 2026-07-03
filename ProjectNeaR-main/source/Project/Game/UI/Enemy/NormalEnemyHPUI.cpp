#include "NormalEnemyHPUI.h"
#include "../../../General/AssetManager.h"
#include "../../../General/CharaStatus.h"
#include "../../Actor/Character/Enemy/EnemyBase.h"
#include <DxLib.h>
#include <cassert>

namespace
{
	//バーの変動フレーム
	constexpr int kFluctuationValueFrame = 120;
	//高さ調整
	constexpr float kPosOffsetY = 50.0f;
	//バーの画像サイズ
	constexpr float kBarSizeX = 100.0f;
	constexpr float kBarSizeY = 10.0f;
	//体力に変動があった場合10秒間表示
	constexpr int kChangeHPFrame = 60 * 10;

	//背景
	constexpr float kBackBarSizeX = 102.0f;
	constexpr float kBackBarSizeY = 12.0f;
	constexpr float kBackPosOffsetY = kPosOffsetY + 1.0f;

	//ハンドル
	const std::wstring kHPValue = L"Enemy/HPValue";
	const std::wstring kHPValueBack = L"Enemy/HPValueBack";
	const std::wstring kHPValueDamage = L"Enemy/HPValueDamage";
	const std::wstring kHPValueHeal = L"Enemy/HPValueHeal";
}

NormalEnemyHPUI::NormalEnemyHPUI(std::shared_ptr<CharaStatus> charaStatus, std::weak_ptr<EnemyBase> pEnemy) :
	HPUIBase(charaStatus),
	m_pEnemy(pEnemy),
	m_changeHPCountFrame(0)

{
	auto& assetManager = AssetManager::GetInstance();
	m_nowValueHandle = assetManager.GetImageHandle(kHPValue);
	m_backValueHandle = assetManager.GetImageHandle(kHPValueBack);
	m_damageValueHandle = assetManager.GetImageHandle(kHPValueDamage);
	m_healValueHandle = assetManager.GetImageHandle(kHPValueHeal);
	m_beforeNowHP = m_charaStatus->GetNowHP();
	m_nowHpRate = m_charaStatus->GetHPRate();
	m_damageValueRate = m_charaStatus->GetHPRate();
	m_healValueRate = m_charaStatus->GetHPRate();
}

NormalEnemyHPUI::~NormalEnemyHPUI()
{
}

void NormalEnemyHPUI::Update()
{
	if (m_pEnemy.expired())
	{
		m_isDelete = true;
		return;
	}
	auto enemy = m_pEnemy.lock();
	if (!enemy->IsActive())return;

	//カウントを減らす
	if (m_changeHPCountFrame > 0)
	{
		--m_changeHPCountFrame;
	}

	float nowHP = m_charaStatus->GetNowHP();
	//体力が前のフレームより低かったら
	if (nowHP < m_beforeNowHP)
	{
		//ダメージを受けた
		m_damageFrame = kFluctuationValueFrame;
		//回復バーは体力に合わせる
		m_healValueRate = m_charaStatus->GetHPRate();
		//体力に変動があったので
		m_changeHPCountFrame = kChangeHPFrame;
	}
	else if (nowHP > m_beforeNowHP)
	{
		//回復
		m_healFrame = kFluctuationValueFrame;
		//回復バーは体力に合わせる
		m_healValueRate = m_charaStatus->GetHPRate();
		//体力に変動があったので
		m_changeHPCountFrame = kChangeHPFrame;
	}
	//現在の体力を更新
	m_beforeNowHP = nowHP;

	//ダメージを受けた
	UpdateDamageBar();

	//体力が回復
	UpdateHealBar();

	//座標更新
	VECTOR enemyPos = enemy->GetHeadPos().ToDxLibVector();
	m_enemyViewPos = ConvWorldPosToScreenPos(enemyPos);

	//カメラに入ってるか
	m_isDraw = !CheckCameraViewClip(enemyPos);
}

void NormalEnemyHPUI::Draw() const
{
	if (m_pEnemy.expired())return;
	auto enemy = m_pEnemy.lock();
	if (!enemy->IsActive())return;
	//カメラの範囲外の時は描画しない
	if (!m_isDraw)return;
	if (m_changeHPCountFrame <= 0)return;

	auto backUIPos = m_enemyViewPos;
	backUIPos.x -= (kBackBarSizeX * 0.5f);
	backUIPos.y -= kBackPosOffsetY;
	//バーの描画
	DrawRectGraphF2(backUIPos.x, backUIPos.y, 0, 0, kBackBarSizeX, kBackBarSizeY, m_backValueHandle, true);

	auto uiPos = m_enemyViewPos;
	uiPos.x -= (kBarSizeX * 0.5f);
	uiPos.y -= kPosOffsetY;

	DrawRectGraphF2(uiPos.x, uiPos.y, 0, 0, kBarSizeX * m_damageValueRate, kBarSizeY, m_damageValueHandle, true);
	DrawRectGraphF2(uiPos.x, uiPos.y, 0, 0, kBarSizeX * m_healValueRate, kBarSizeY, m_healValueHandle, true);
	DrawRectGraphF2(uiPos.x, uiPos.y, 0, 0, kBarSizeX * m_nowHpRate, kBarSizeY, kBarSizeY, m_nowValueHandle, true);
}
