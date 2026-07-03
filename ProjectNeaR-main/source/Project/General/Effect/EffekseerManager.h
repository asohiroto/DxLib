#pragma once
#include <memory>
#include <list>
#include <map>
#include <string>
#include "../Math/MyMath.h"

class NormalEffect;
class TrackActorEffect;
class Actor;
/// <summary>
/// エフェクトを管理するシングルトンクラス
/// </summary>
class EffekseerManager final
{
private:
	//シングルトンの準備
	EffekseerManager() = default;
	~EffekseerManager() = default;
	//コピー禁止
	EffekseerManager(const EffekseerManager&) = delete;
	EffekseerManager& operator = (const EffekseerManager&) = delete;
	//ムーブ禁止
	EffekseerManager(EffekseerManager&&) = delete;
	EffekseerManager& operator = (EffekseerManager&&) = delete;
public:
	//インスタンスを取得
	static EffekseerManager& GetInstance()
	{
		static EffekseerManager instance;
		return instance;
	}
	//Effekseer関連
	void InitEffekseer();
	void EndEffekseer();


	//登録
	void Entry(std::shared_ptr<NormalEffect> eff);
	//解除
	void Exit(std::shared_ptr<NormalEffect> eff);
	//初期化
	void Init();
	//更新処理
	void Update();
	//描画
	void Draw()const;
	//終了処理
	void End();
	//すべてのエフェクトを削除
	void Reset();
	//エフェクトを作成
	std::weak_ptr<NormalEffect> CreateEffect(std::wstring path, Vector3 pos);
	std::weak_ptr<TrackActorEffect> CreateTrackActorEffect(std::wstring path, std::weak_ptr<Actor> actor);
	//エフェクトを止める
	void StopEffect();
	//エフェクトを開始
	void StartEffect();
	//数フレーム止める
	void DelayUpdate(int frame);
private:
	//エフェクトの配列
	std::list<std::shared_ptr<NormalEffect>> m_effects;
	//更新するか
	bool m_isUpdate = true;
	//更新を遅らせる
	int m_delayFrame = 0;
private:
	//削除候補のエフェクトを削除
	void CheckDeleteEffect();
	//更新を遅延させるフレームの処理
	void UpdateDelay();
};

