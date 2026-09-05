#pragma once
#include <memory>

class EnemyHpBar;
class PlayerHpBar;
class PlayerMpBar;
class PlayerUltGauge;

class UIManager
{
public:
	UIManager();
	virtual ~UIManager();
	void Init(float eneMaxHp, float playMaxHp,
		float playMaxMp, float maxCharge,
		int pHpH, int eHpH, int ultH);
	void End();
	void Update(float eneNowHp, float playNowHp, float playNowMp, float nowCharge);
	void Draw();

private:
	// 画面中央に照準用のクロスヘアを描画する
	void DrawCrosshair();

private:
	// 敵HPバー
	std::shared_ptr<EnemyHpBar> p_EneHpBar;
	// プレイヤーHPバー
	std::shared_ptr<PlayerHpBar> p_PlayHpBar;
	// プレイヤーMPバー
	std::shared_ptr<PlayerMpBar> p_PlayMpBar;
	// プレイヤー必殺技ゲージ
	std::shared_ptr<PlayerUltGauge> p_PlayUltGauge;
};