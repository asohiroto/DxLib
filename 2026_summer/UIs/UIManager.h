#pragma once
#include "EnemyHpBar.h"
#include "PlayerHpBar.h"
#include "PlayerMpBar.h"
#include "PlayerUltGauge.h"

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
	EnemyHpBar _EneHpBar;
	// プレイヤーHPバー
	PlayerHpBar _PlayHpBar;
	// プレイヤーMPバー
	PlayerMpBar _PlayMpBar;
	// プレイヤー必殺技ゲージ
	PlayerUltGauge _PlayUltGauge;
	// クロスヘアを赤く表示する残りフレーム数
	int _crosshairRedTimer;
	// 操作ガイドのYボタン(回避)のハンドル
	int _guideYButtonH;
	// 操作ガイドのBボタン(ショット/ミサイル)のハンドル
	int _guideBButtonH;
	// 操作ガイドのAボタン(必殺技)のハンドル
	int _guideAButtonH;
};