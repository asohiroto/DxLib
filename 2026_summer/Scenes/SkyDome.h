#pragma once

class SkyDome
{
public:
	SkyDome();
	virtual ~SkyDome();
	void Init(int handle);
	void End();
	void Update();
	void Draw();

private:
	int _domeH;
};