#pragma once

class PlayerHpBar
{
public:
	PlayerHpBar();
	virtual ~PlayerHpBar();
	void Init(float maxHp, int handle);
	void End();
	void Update(float nowHp);
	void Draw();

private:
	// HPバーを描画
	void DrawHpBar(float rate, int color);

private:
	// 現在のHP
	float _nowHp;
	// 表示用に補間しているHP
	float _dispHp;
	// 最大HP
	float _maxHp;
	// 現在のHP割合
	float _nowRate;
	// 表示用に補間しているHP割合
	float _dispRate;

	// アイコン画像のハンドル
	int _playerHpBarH;
};