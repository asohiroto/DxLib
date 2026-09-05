#pragma once
#include <DxLib.h>

class SkyDome
{
public:
	SkyDome();
	virtual ~SkyDome();
	void Init(int handle);
	void End();
	void Update();
	void Draw();
	// ドームを回転させる
	void SetRotate(float angle) { MV1SetRotationXYZ(_domeH, VGet(0.0f, angle, 0.0f)); }

private:
	// ドームのモデルハンドル
	int _domeH;
};