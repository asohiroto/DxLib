#include "EffectManager.h"

namespace
{
	// 画面に表示できる最大パーティクル数
	const int	EffectParticleLimit = 20000;
	// エフェクトのファイルパス
	const char* EffectFilePath = "data/flame/Flame.efk";
	// エフェクトのサイズ
	const float EffectSize = 1.0f;
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
	// 引数には画面に表示する最大パーティクル数を設定する。
	if (Effkseer_Init(EffectParticleLimit) == -1)
	{
		DxLib_End();
	}

	// DXライブラリのデバイスロストした時のコールバックを設定する。
	// ウインドウとフルスクリーンの切り替えが発生する場合は必ず実行する。
	Effekseer_SetGraphicsDeviceLostCallbackFunctions();
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
}

