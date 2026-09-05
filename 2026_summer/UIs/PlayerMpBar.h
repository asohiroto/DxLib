#pragma once

class PlayerMpBar
{
public:
	PlayerMpBar();
	virtual ~PlayerMpBar();
	void Init(float maxMp);
	void End();
	void Update(float nowMp);
	void Draw();

private:
	// MPバーを描画する
	void DrawMpBar(float rate, int color);

private:
	// 現在のMP
	float _nowMp;
	// 表示用に補間しているMP
	float _dispMp;
	// 最大MP
	float _maxMp;
	// 現在のMP割合
	float _nowRate;
	// 表示用に補間しているMP割合
	float _dispRate;
};