#pragma once
#include "GameObject.h"
#include<DxLib.h>

class Ground :
	public GameObject
{
public:
	Ground();
	~Ground();
	void Init() override;
	void Update() override;
	void Draw() const override;

	void SetPos(VECTOR pos) override { MV1SetPosition(_modelH, pos); }
	void SetRot(VECTOR rot) { MV1SetRotationXYZ(_modelH, rot); }
	void SetScale(VECTOR scale) { MV1SetScale(_modelH, scale); }

private:
	int _modelH = -1;
};

