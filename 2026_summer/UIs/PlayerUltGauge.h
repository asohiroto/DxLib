#pragma once

class PlayerUltGauge
{
public:
	PlayerUltGauge();
	virtual ~PlayerUltGauge();
	void Init(float maxCharge);
	void End();
	void Update(float nowCharge);
	void Draw();
	// 必殺技のゲージを描画
	void DrawUltGauge(float rate, int color);

private:
	float _nowCharge;
	float _dispCharge;
	float _maxCharge;
	float _nowRate;
	float _dispRate;
};