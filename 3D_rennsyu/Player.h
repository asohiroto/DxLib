#pragma once
#pragma once
#include "GameObject.h"

class Player :
	public GameObject
{
public:
	Player();
	~Player();
	void Init() override;
	void Update() override;
	void Draw() const override;

private:
	int _modelH = -1;
	int _animH = -1;
	VECTOR _move;
};

