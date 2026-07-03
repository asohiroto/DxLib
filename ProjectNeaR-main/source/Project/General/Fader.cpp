#include "Fader.h"
#include "game.h"
#include "Input.h"
#include <DxLib.h>

namespace
{
	constexpr unsigned int kColor = 0x000000;
}

//初期化
void Fader::Init()
{
	m_isFadeIn = false;
	m_isFadeOut = false;
	m_countFrame = 0.0f;
	m_fadeSpeed = 10.0f;
	m_color = kColor;
}

//更新
void Fader::Update()
{
	auto& input = Input::GetInstance();
	if (m_isFadeIn)
	{
		//入力を止める
		input.StopUpdate();
		//だんだん明るくなる
		m_countFrame -= m_fadeSpeed;
		if (m_countFrame <= 0.0f)
		{
			//入力を開始
			input.StartUpdate();
			m_countFrame = 0.0f;
			m_isFadeIn = false;
		}
	}
	if (m_isFadeOut)
	{
		//入力を止める
		input.StopUpdate();
		//だんだん暗くなる
		m_countFrame += m_fadeSpeed;
		if (m_countFrame >= 255.0f)
		{
			//入力を開始
			input.StartUpdate();
			m_countFrame = 255.0f;
			m_isFadeOut = false;
		}
	}
	//フェード中なら
	if (m_isFadeIn || m_isFadeOut)
	{
		m_isFadeNow = true;
	}
	else
	{
		m_isFadeNow = false;
	}
}
//描画
void Fader::Draw()const
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(m_countFrame));
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, m_color, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);
}

void Fader::FadeIn(float speed)
{
	if (!m_isFadeIn)
	{
		//だんだん明るくなる
		m_countFrame = 255.0f;
		m_fadeSpeed = speed;
		m_isFadeIn = true;
		m_isFadeOut = false;
	}
}

void Fader::FadeOut(float speed)
{
	if (!m_isFadeOut)
	{
		//だんだん暗くなる
		m_countFrame = 0.0f;
		m_fadeSpeed = speed;
		m_isFadeOut = true;
		m_isFadeIn = false;
	}
}

bool Fader::IsFinishFadeIn()const
{
	return m_countFrame <= 0.0f && !m_isFadeOut;
}


bool Fader::IsFinishFadeOut() const
{
	return  m_countFrame >= 255.0f && !m_isFadeIn;
}