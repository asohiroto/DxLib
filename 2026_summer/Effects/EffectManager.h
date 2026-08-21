#pragma once
#include <EffekseerForDXLib.h> 

class EffectManager
{
public:
	EffectManager();
	~EffectManager();
	void Init();
	void Load();
	void Update(VECTOR playPosition);
	void Draw();
private:
	// エフェクトのリソース用
	int effectResourceHandle;
	// 再生中のエフェクトハンドル
	int playingEffectHandle;
	// 周期敵に再生するためのカウント
	int	playCount;
};


