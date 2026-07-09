#pragma once
#include <DxLib.h>

class PlayerMove
{
public:
	PlayerMove() {};
	virtual ~PlayerMove() {};
	void Init() {};
	void Update() {};
	void Draw() {};

	//void WeakAttack();
	//void StrongAttack();
	//void Dodge();
	void Move(VECTOR& pos, VECTOR& move, int inputX, int inputZ, float cameraAngle, int& modelH);
};