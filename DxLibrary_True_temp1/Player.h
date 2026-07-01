#pragma once

namespace
{
	constexpr int kStartX = 300;
	constexpr int kStartY = 300;

	constexpr int kRadius = 24;
}

class Player
{
public:
	Player();
	~Player();
	void Init();
	void Update();
	void Draw();
private:
	int _posX, _posY;
};

