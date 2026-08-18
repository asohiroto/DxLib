#pragma once

class EnemyHpBar
{
public:
	EnemyHpBar();
	virtual ~EnemyHpBar();
	void Init(float maxHp);
	void End();
	void Update(float nowHp);
	void Draw();
	// HPバーを描画
	void DrawHpBar(float rate, int color);

private:
	float _nowHp;
	float _dispHp;
	float _maxHp;
	float _nowRate;
	float _dispRate;
};