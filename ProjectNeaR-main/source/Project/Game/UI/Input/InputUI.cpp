#include "InputUI.h"
#include "../../../General/Game.h"
#include "../../../General/AssetManager.h"
#include <string>
#include <DxLib.h>

namespace
{
	const std::wstring kInputPath = L"Input/InputUI";

	//ç¿ïW
	constexpr int kPosX = Game::kScreenWidth - 250;
	constexpr int kPosY = 12;
}

InputUI::InputUI():
	m_handle(-1)
{
	m_handle = AssetManager::GetInstance().GetImageHandle(kInputPath);
}

InputUI::~InputUI()
{
}

void InputUI::Update()
{
}

void InputUI::Draw() const
{
	if (!m_isDraw)return;
	DrawGraph(kPosX, kPosY, m_handle, true);
}
