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
	void Update(float eneNowHp, float playNowHp, float playNowMp, float nowCharge, bool isEnemyHit);
	void Draw();

private:
	// 画面中央に照準用のクロスヘアを描画する
	void DrawCrosshair();
	// 画面左下に簡易操作ガイドを描画する
	void DrawControlGuide();

private:
	// 敵HPバー
	std::shared_ptr<EnemyHpBar> p_EneHpBar;
	// プレイヤーHPバー
	std::shared_ptr<PlayerHpBar> p_PlayHpBar;
	// プレイヤーMPバー
	std::shared_ptr<PlayerMpBar> p_PlayMpBar;
	// プレイヤー必殺技ゲージ
	std::shared_ptr<PlayerUltGauge> p_PlayUltGauge;
	// クロスヘアを赤く表示する残りフレーム数
	int _crosshairRedTimer;
	// 操作ガイドのYボタン(回避)のハンドル
	int _guideYButtonH;
	// 操作ガイドのBボタン(ショット/ミサイル)のハンドル
	int _guideBButtonH;
	// 操作ガイドのAボタン(必殺技)のハンドル
	int _guideAButtonH;
};