#include "EffectManager.h"

namespace
{
	// エフェクトのファイルパス
	const char* EffectFilePath = "data/flame/Flame.efk";
	// エフェクトのサイズ
	const float EffectSize = 5.0f;
	// エフェクトを再生する周期
	const int	EffectPlayInterval = 300;
	// エフェクトが移動する速度
	const float	EffectMoveSpeed = 0.2f;
}

EffectManager::EffectManager() :
	effectResourceHandle(-1),
	playingEffectHandle(-1),
	playCount(0)
{
}

EffectManager::~EffectManager()
{
	// エフェクトリソースの開放
	DeleteEffekseerEffect(effectResourceHandle);
}

void EffectManager::Init()
{

}

// 読み込み
void EffectManager::Load()
{
	// エフェクトのリソースを読み込む
	effectResourceHandle = LoadEffekseerEffect(EffectFilePath, EffectSize);
}

/// <summary>
/// 更新
/// </summary>
/// <param name="playPosition">再生座標</param>
void EffectManager::Update(VECTOR playPosition)
{
	// 定期的にエフェクトを再生する
	if (!(playCount % EffectPlayInterval))
	{
		// エフェクトを再生する。
		playingEffectHandle = PlayEffekseer3DEffect(effectResourceHandle);
	}

	// 再生カウントを進める
	playCount++;

	// 再生中のエフェクトを移動する。
	SetPosPlayingEffekseer3DEffect(playingEffectHandle, playPosition.x, playPosition.y, playPosition.z);

	// Effekseerにより再生中のエフェクトを更新する。
	UpdateEffekseer3D();
}

// 描画
void EffectManager::Draw()
{
	// Effekseerにより再生中のエフェクトを描画する。
	DrawEffekseer3D();

	DrawFormatString(100, 300, 0x000000, "%d", playingEffectHandle);
}

