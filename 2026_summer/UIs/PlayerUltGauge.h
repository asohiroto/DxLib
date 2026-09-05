#pragma once

class PlayerUltGauge
{
public:
	PlayerUltGauge();
	virtual ~PlayerUltGauge();
	void Init(float maxCharge, int handle);
	void End();
	void Update(float nowCharge);
	void Draw();

private:
	// 必殺技のゲージを描画
	void DrawUltGauge(float rate, int color);

private:
	// 現在の必殺技チャージ量
	float _nowCharge;
	// 表示用に補間しているチャージ量
	float _dispCharge;
	// 最大チャージ量
	float _maxCharge;
	// 現在のチャージ割合
	float _nowRate;
	// 表示用に補間しているチャージ割合
	float _dispRate;

	// アイコン画像のハンドル
	int _ultGaugeH;
};