#include "BottunUI.h"
#include "../../../General/AssetManager.h"
#include "../../../General/Input.h"
#include "../../../General/StringUtil.h"

namespace
{
	//ƒpƒX
	const std::wstring kBottunPath = L"Input/";
	const std::wstring kNoPushBottunPath = L"1";
	const std::wstring kPushBottunPath = L"2";
}

BottunUI::BottunUI(std::wstring bottun):
	UIBase(),
	m_noPushBottunHandle(-1),
	m_pushBottunHandle(-1),
	m_pos(),
	m_wBottun(bottun),
	m_sBottun(StringUtil::WstringToString(bottun))
{
	auto& assetManager = AssetManager::GetInstance();
	m_noPushBottunHandle = assetManager.GetImageHandle(kBottunPath + bottun + kNoPushBottunPath);
	m_pushBottunHandle = assetManager.GetImageHandle(kBottunPath + bottun + kPushBottunPath);
}

BottunUI::~BottunUI()
{
}

void BottunUI::Update()
{
}

void BottunUI::Draw() const
{
	if (!m_isDraw)return;
	int handle = m_noPushBottunHandle;
	if (Input::GetInstance().IsPress(m_sBottun.c_str()))
	{
		handle = m_pushBottunHandle;
	}

	DrawRotaGraphF(m_pos.x, m_pos.y, 1.0f, 0.0f, handle, true);
}
