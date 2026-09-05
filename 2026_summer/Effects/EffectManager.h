#pragma once
#include <EffekseerForDXLib.h> 

class EffectManager
{
public:
	EffectManager();
	~EffectManager();
	void Init();
	// エフェクトのリソースを読み込む
	void Load();
	void Update();
	void Draw();
private:
};


