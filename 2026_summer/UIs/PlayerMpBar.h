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
	// MPバーを描画する
	void DrawMpBar(float rate, int color);

private:
	float _nowMp;
	float _dispMp;
	float _maxMp;
	float _nowRate;
	float _dispRate;
};