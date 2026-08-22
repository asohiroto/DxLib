#include "EffectManager.h"

namespace
{

}

EffectManager::EffectManager()
{
}

EffectManager::~EffectManager()
{

}

void EffectManager::Init()
{

}

// 読み込み
void EffectManager::Load()
{

}

void EffectManager::Update()
{
	// Effekseerにより再生中のエフェクトを更新する。
	UpdateEffekseer3D();
}

// 描画
void EffectManager::Draw()
{
	// DxLibのカメラ設定をEffekseerに同期する
	Effekseer_Sync3DSetting();

	// Effekseerにより再生中のエフェクトを描画する。
	DrawEffekseer3D();

}

