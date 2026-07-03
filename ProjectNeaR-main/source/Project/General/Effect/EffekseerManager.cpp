#include "EffekseerManager.h"
#include "NormalEffect.h"
#include "TrackActorEffect.h"
#include <EffekseerForDXLib.h>
#include "../../Main/Application.h"
#include "../AssetManager.h"

namespace
{
	constexpr int kMaxParticleNum = 8000;
}

void EffekseerManager::InitEffekseer()
{
	//パーティクル数を設定
	Effekseer_Init(kMaxParticleNum);
	//歪みエフェクトの初期化
	Effekseer_InitDistortion();
	//フルスク切り替え時におかしくならないように設定
	Effekseer_SetGraphicsDeviceLostCallbackFunctions();
}

void EffekseerManager::EndEffekseer()
{
	//終了
	Effkseer_End();
}

void EffekseerManager::Entry(std::shared_ptr<NormalEffect> eff)
{
	//すでに登録されているならしない
	auto it = std::find(m_effects.begin(), m_effects.end(), eff);
	if (it != m_effects.end())return;
	//登録
	m_effects.emplace_back(eff);
}

void EffekseerManager::Exit(std::shared_ptr<NormalEffect> eff)
{
	//登録されていないならしない
	auto it = std::find(m_effects.begin(), m_effects.end(), eff);
	if (it == m_effects.end())return;
	//解除
	m_effects.remove(eff);
}
void EffekseerManager::Init()
{
	//初期化
	m_isUpdate = true;
	m_delayFrame = 0;
}

void EffekseerManager::Update()
{
	//遅延処理
	UpdateDelay();
	//更新をしないなら
	if (!m_isUpdate || m_delayFrame > 0)
	{
		//再生ストップ
		StopEffect();
		return;
	}
	else
	{
		//再生
		StartEffect();
	}
	//更新
	for (auto& eff : m_effects)
	{
		eff->Update();
	}
	//削除予定のものを削除
	CheckDeleteEffect();
	//全てのエフェクトの更新
	UpdateEffekseer3D();
}

void EffekseerManager::Draw() const
{
	//Effekseerに3D表示の設定をDXライブラリの3D表示の設定に同期させる
	Effekseer_Sync3DSetting();
	//全てのエフェクトの描画
	DrawEffekseer3D_Begin();
	for (auto& eff : m_effects)
	{
		if (!eff->IsDraw())continue;
		int playHandle = eff->GetPlayHandle();
		if (playHandle == -1)continue;
		DrawEffekseer3D_Draw(playHandle);
	}
	DrawEffekseer3D_End();
}

void EffekseerManager::End()
{
	//エフェクト削除
	Reset();
}

void EffekseerManager::Reset()
{
	//削除
	for (auto& eff : m_effects)
	{
		eff->End();
	}
	m_effects.clear();
	
}

std::weak_ptr<NormalEffect> EffekseerManager::CreateEffect(std::wstring path, Vector3 pos)
{
	std::shared_ptr<NormalEffect> effect;
	effect = std::make_shared<NormalEffect>(AssetManager::GetInstance().GetEffectHandle(path), pos);
	Entry(effect);
	return effect;
}

std::weak_ptr<TrackActorEffect> EffekseerManager::CreateTrackActorEffect(std::wstring path, std::weak_ptr<Actor> actor)
{
	std::shared_ptr<TrackActorEffect> effect;
	effect = std::make_shared<TrackActorEffect>(AssetManager::GetInstance().GetEffectHandle(path), actor);
	Entry(effect);
	return effect;
}

void EffekseerManager::StopEffect()
{
	//止める
	m_isUpdate = false;
}

void EffekseerManager::StartEffect()
{
	//再生
	m_isUpdate = true;
}

void EffekseerManager::DelayUpdate(int frame)
{
	m_delayFrame = frame;
	m_isUpdate = false;
}

//エフェクトの消滅フラグをチェックして削除
void EffekseerManager::CheckDeleteEffect()
{
	std::list<std::shared_ptr<NormalEffect>> deleteEffect;
	for (int i = 0;i < 3;++i)
	{
		bool isOneMore = false;
		for (auto& eff : m_effects)
		{
			if (eff->IsDelete())
			{
				isOneMore = true;
				//終了処理
				eff->End();
				//削除候補
				deleteEffect.emplace_back(eff);
			}
		}
		//削除
		for (auto& eff : deleteEffect)
		{
			Exit(eff);
		}
		deleteEffect.clear();
		if (!isOneMore)break;
	}
}
void EffekseerManager::UpdateDelay()
{
	if (m_delayFrame > 0)
	{
		m_delayFrame -= Application::GetInstance().GetTimeScale();
		if (m_delayFrame <= 0)
		{
			m_isUpdate = true;
		}
	}
}