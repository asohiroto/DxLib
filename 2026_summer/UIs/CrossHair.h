#pragma once

class CrossHair
{
public:
	CrossHair();
	virtual ~CrossHair();
	void Init();
	void End();
	void Update();
	void Draw();
};