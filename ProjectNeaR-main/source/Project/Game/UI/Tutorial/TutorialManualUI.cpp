#include "TutorialManualUI.h"
#include "../../../General/AssetManager.h"
#include "../../../General/CharaStatus.h"
#include "../../../General/Math/MyMath.h"
#include "../../../General/Game.h"
#include <DxLib.h>

namespace
{
	//ハンドル
	const std::wstring kT1HandlePath = L"Tutorial/Tutorial1UI";
	const std::wstring kT2HandlePath = L"Tutorial/Tutorial2UI";
	const std::wstring kT3HandlePath = L"Tutorial/Tutorial3UI";

	//目標座標
	const Vector2 kFirstPos = { Game::kScreenWidth + 400, 20 };
	const Vector2 kNextPos = { Game::kScreenWidth - 400, 20 };
	//Lerp
	constexpr float kLerpRate = 0.1f;
}

TutorialManualUI::TutorialManualUI(StageIndex index) :
	UIBase(),
	m_handle(-1),
	m_pos(kFirstPos)
{
	std::wstring path = kT1HandlePath;
	switch (index)
	{
	case StageIndex::Tutorial1:
		path = kT1HandlePath;
		break;
	case StageIndex::Tutorial2:
		path = kT2HandlePath;
		break;
	case StageIndex::Tutorial3:
		path = kT3HandlePath;
		break;
	default:
		break;
	}

	//ハンドル
	m_handle = AssetManager::GetInstance().GetImageHandle(path.c_str());
	//前描画
	m_isFrontDraw = true;
}

TutorialManualUI::~TutorialManualUI()
{
}

void TutorialManualUI::Update()
{
	if (!m_isDraw)return;
	m_pos = Vector2::Lerp(m_pos, kNextPos, kLerpRate);
}

void TutorialManualUI::Draw() const
{
	if (!m_isDraw)return;
	//タイトル背景
	DrawGraph(m_pos.x, m_pos.y, m_handle, true);
}
