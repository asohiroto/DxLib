#include "Animator.h"
#include "../Main/Application.h"
#include <DxLib.h>
namespace
{
	constexpr float kMaxBlend = 1.0f;//ブレンド率の最大
	constexpr float kBlendRate = 0.1f;//ブレンド率
	constexpr float kDefAnimSpeed = 0.5f;//デフォルトの再生速度
}

Animator::Animator() :
	m_blendRate(kMaxBlend),
	m_timeScale(1.0f)
{
}

Animator::~Animator()
{
}

void Animator::SetAnim(const int& modelHandle, const int& anim, const bool& isLoop)
{
	//メインかサブのアニメーションと同じなら設定しない
	if (anim == m_animNext.m_attachAnim)return;
	//タイムスケールが0の時も設定しない
	if (m_timeScale <= 0.0f)return;

	if (m_animNow.m_attachAnim != -1)
	{
		//古いアニメーションは消す
		RemoveAnim(modelHandle, m_animNow);
	}
	//新しいアニメーションを古いアニメーションにする
	m_animNow = m_animNext;

	//新しいモーションに更新
	m_animNext.m_attachAnimIndex = MV1AttachAnim(modelHandle, anim, -1, false);
	m_animNext.m_attachAnim = anim;//今のアニメーションの番号
	m_animNext.m_animStopTime = MV1GetAttachAnimTotalTime(modelHandle, m_animNext.m_attachAnimIndex);//アニメーションの終了時間
	m_animNext.m_animTimer = 0.0f;//タイマー初期化
	m_animNext.m_isLoopAnim = isLoop;//ループするか
	m_animNext.m_isFinishAnim = false;//再生終了
	m_animNext.m_animSpeed = kDefAnimSpeed;//再生速度
	//指定ループ
	m_animNext.m_isFixedLoop = false;
	m_animNext.m_fixedLoopFrame = 0.0f;

	//ブレンド率リセット
	m_blendRate = 0.0f;

	//ブレンド
	MV1SetAttachAnimBlendRate(modelHandle, m_animNow.m_attachAnim, kMaxBlend - m_blendRate);
	MV1SetAttachAnimBlendRate(modelHandle, m_animNext.m_attachAnim, m_blendRate);
}

void Animator::SetAnim(const int& modelHandle, const int& anim, const bool& isLoop, const float& animSpeed)
{
	//メインかサブのアニメーションと同じなら設定しない
	if (anim == m_animNext.m_attachAnim)return;
	//タイムスケールが0の時も設定しない
	if (m_timeScale <= 0.0f)return;

	if (m_animNow.m_attachAnim != -1)
	{
		//古いアニメーションは消す
		RemoveAnim(modelHandle, m_animNow);
	}
	//新しいアニメーションを古いアニメーションにする
	m_animNow = m_animNext;

	//新しいモーションに更新
	m_animNext.m_attachAnimIndex = MV1AttachAnim(modelHandle, anim, -1, false);
	m_animNext.m_attachAnim = anim;//今のアニメーションの番号
	m_animNext.m_animStopTime = MV1GetAttachAnimTotalTime(modelHandle, m_animNext.m_attachAnimIndex);//アニメーションの終了時間
	m_animNext.m_animTimer = 0.0f;//タイマー初期化
	m_animNext.m_isLoopAnim = isLoop;//ループするか
	m_animNext.m_isFinishAnim = false;//再生終了
	m_animNext.m_animSpeed = animSpeed;//再生速度
	//指定ループ
	m_animNext.m_isFixedLoop = false;
	m_animNext.m_fixedLoopFrame = 0.0f;
	//ブレンド率リセット
	m_blendRate = 0.0f;

	//ブレンド
	MV1SetAttachAnimBlendRate(modelHandle, m_animNow.m_attachAnim, kMaxBlend - m_blendRate);
	MV1SetAttachAnimBlendRate(modelHandle, m_animNext.m_attachAnim, m_blendRate);
}

void Animator::Replay()
{
	//最初から再生
	m_animNext.m_animTimer = 0.0f;
	m_blendRate = 1.0f;
	m_animNext.m_isFinishAnim = false;
}

void Animator::RemoveAnim(const int& modelHandle, Anim& anim)
{
	//そもそも何もアタッチされていないなら早期リターン
	if (anim.m_attachAnimIndex == -1)return;

	//今のモーションを消す
	MV1DetachAnim(modelHandle, anim.m_attachAnimIndex);
	anim.m_attachAnim = -1;
	anim.m_attachAnimIndex = -1;
	anim.m_animStopTime = 0.0f;
	anim.m_animTimer = 0.0f;
	anim.m_isLoopAnim = false;
	anim.m_isFinishAnim = false;
	anim.m_isFixedLoop = false;
	anim.m_fixedLoopFrame = 0.0f;
}

void Animator::AllRemoveAnim(const int& modelHandle)
{
	RemoveAnim(modelHandle, m_animNow);
	RemoveAnim(modelHandle, m_animNext);
}

void Animator::PlayAnim(const int& modelHandle)
{
	//更新
	UpdateBlend(modelHandle);
	UpdateAnim(modelHandle, m_animNow);
	UpdateAnim(modelHandle, m_animNext);
}

bool Animator::IsFinishAnim()
{
	return m_animNext.m_isFinishAnim;
}

void Animator::SetFixedLoop(float loopFrame)
{
	//指定ループ
	m_animNext.m_isFixedLoop = true;
	m_animNext.m_fixedLoopFrame = loopFrame;
}

bool Animator::IsFinishFixedLoop()
{
	//指定ループ再生が終了したらtrue
	return m_animNext.m_isFixedLoop && m_animNext.m_fixedLoopFrame <= 0.0f;
}

void Animator::SetAnimSpeed(const float& animSpeed)
{
	m_animNext.m_animSpeed = animSpeed;
}

void Animator::UpdateAnim(const int& modelHandle, Anim& anim)
{
	//何もアタッチされてないなら再生しない
	if (anim.m_attachAnimIndex == -1)return;

	//アニメーションが終わっていないかもしれないのでfalse
	anim.m_isFinishAnim = false;
	//アニメーションの終わりまで再生(指定ループ再生の場合指定ループフレームが終了したら)
	if ((anim.m_animStopTime <= anim.m_animTimer) || (anim.m_isFixedLoop && anim.m_fixedLoopFrame <= 0.0f))
	{
		//ループする(指定ループ再生の場合指定ループフレームが終了していないなら)
		if (anim.m_isLoopAnim || (anim.m_isFixedLoop && anim.m_fixedLoopFrame > 0.0f))
		{
			//最初から
			anim.m_animTimer = 0.0f;
			//終わりがないので終わったことにしておく
			anim.m_isFinishAnim = true;
		}
		else
		{
			//終わったらtrue
			anim.m_isFinishAnim = true;
		}
	}
	//アニメーションを進める
	MV1SetAttachAnimTime(modelHandle, anim.m_attachAnimIndex, anim.m_animTimer);
	anim.m_animTimer += anim.m_animSpeed * m_timeScale;
	//指定ループ再生なら
	if (anim.m_isFixedLoop && anim.m_fixedLoopFrame > 0.0f)
	{
		--anim.m_fixedLoopFrame;//ループフレームを減らしていく
	}
}

void Animator::UpdateBlend(const int& modelHandle)
{
	//ブレンド
	MV1SetAttachAnimBlendRate(modelHandle, m_animNow.m_attachAnimIndex, kMaxBlend - m_blendRate);
	MV1SetAttachAnimBlendRate(modelHandle, m_animNext.m_attachAnimIndex, m_blendRate);
	//だんだん変化していく
	m_blendRate += kBlendRate * m_timeScale;
	//ブレンド率が最大なら
	if (m_blendRate >= kMaxBlend)
	{
		m_blendRate = kMaxBlend;
	}
}
