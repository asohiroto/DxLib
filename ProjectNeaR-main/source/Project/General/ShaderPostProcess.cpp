#include "ShaderPostProcess.h"
#include "MyDraw.h"
#include "Math/MyMath.h"
#include "LoadingManager.h"
#include "Game.h"
#include <DxLib.h>

ShaderPostProcess::ShaderPostProcess() :
	m_psHandle(-1),
	m_cbuff1Handle(-1),
	m_cbuff1(nullptr),
	m_countJustFrame(0.0f),
	m_justFrame(1.0f)
{
}

ShaderPostProcess::~ShaderPostProcess()
{
}

void ShaderPostProcess::Init()
{
	//定数バッファ1
	m_cbuff1Handle = CreateShaderConstantBuffer(sizeof(CBuff1));
	m_cbuff1 = static_cast<CBuff1*>(GetBufferShaderConstantBuffer(m_cbuff1Handle));
	m_cbuff1->shakeStrength = 0.0f;
	m_cbuff1->blockScale = 0.0f;
	m_cbuff1->noiseSpeed = 0.0f;
	m_cbuff1->state = static_cast<int>(PostEffectState::Normal);
	//定数バッファ2
	m_cbuff2Handle = CreateShaderConstantBuffer(sizeof(CBuff2));
	m_cbuff2 = static_cast<CBuff2*>(GetBufferShaderConstantBuffer(m_cbuff2Handle));
	m_cbuff2->justRate = 0.0f;

	//設定用
	m_optionCbuff.blockScale = 1.0f;
	m_optionCbuff.noiseSpeed = 1.0f;
	m_optionCbuff.shakeStrength = 1.0f;

	//ポストエフェクト
	m_psHandle = LoadPixelShader(L"Shader/PostEffects.pso");
	assert(m_psHandle >= 0);
}
void ShaderPostProcess::Update()
{
	if (LoadingManager::GetInstance().IsLoading())return;
	if (m_cbuff1 == nullptr)return;
	if (m_cbuff2 == nullptr)return;

	if (m_cbuff1->state & static_cast<int>(PostEffectState::Glitch) || 
		m_cbuff1->state & static_cast<int>(PostEffectState::NoColorGlitch))
	{
		//更新
		m_cbuff1->shakeStrength = MyMath::GetRandF(0.0f, m_optionCbuff.shakeStrength);
		m_cbuff1->blockScale = MyMath::GetRandF(0.0f, m_optionCbuff.blockScale);
		m_cbuff1->noiseSpeed = MyMath::GetRandF(0.0f, m_optionCbuff.noiseSpeed);
	}
	//ジャスト回避をしているなら
	if (m_cbuff1->state & static_cast<int>(PostEffectState::JustAvoid))
	{
		//更新
		m_cbuff2->justRate = m_countJustFrame / m_justFrame;
		if (m_countJustFrame > m_justFrame)
		{
			//ジャスト回避演出終了
			SubPostEffectState(PostEffectState::JustAvoid);
		}
		else
		{
			++m_countJustFrame;
		}
	}
	else
	{
		m_cbuff2->justRate = 0.0f;
		m_countJustFrame = 0.0f;
	}
}

void ShaderPostProcess::Draw(const int rt) const
{
	if (m_psHandle == -1)return;
	if (rt == -1)return;
	MyDraw::DrawPS(Game::kScreenCenterX, Game::kScreenCenterY, 1.0, 0.0, m_cbuff1Handle, m_cbuff2Handle, rt, m_psHandle);
}

void ShaderPostProcess::Draw(float x, float y, const int handle) const
{
	if (m_psHandle == -1)return;
	if (handle == -1)return;
	MyDraw::DrawPS(x, y, 1.0, 0.0, m_cbuff1Handle, m_cbuff2Handle, handle, m_psHandle);
}

void ShaderPostProcess::End()
{
	//解放
	if (m_psHandle > -1)
	{
		DeleteShader(m_psHandle);
	}
	if (m_cbuff1Handle > -1)
	{
		DeleteShaderConstantBuffer(m_cbuff1Handle);
	}
	if (m_cbuff2Handle > -1)
	{
		DeleteShaderConstantBuffer(m_cbuff2Handle);
	}
}

void ShaderPostProcess::SetJustAvoidEffectTime(const float time)
{
	m_justFrame = time;
	if (m_justFrame <= 0.0f)
	{
		m_justFrame = 1.0f;
	}
	m_countJustFrame = 0;
}

void ShaderPostProcess::SetBlockScele(float scale)
{
	m_optionCbuff.blockScale = abs(scale);
}

void ShaderPostProcess::SetNoiseSpeed(float speed)
{
	m_optionCbuff.noiseSpeed = abs(speed);
}

void ShaderPostProcess::SetShakeStrength(float shake)
{
	m_optionCbuff.shakeStrength = abs(shake);
}
