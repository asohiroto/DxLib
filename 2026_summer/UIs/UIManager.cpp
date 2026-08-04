#include "UIManager.h"
#include "CrossHair.h"

UIManager::UIManager() :
	p_CrossHair(nullptr)
{
}

UIManager::~UIManager()
{
}

void UIManager::Init()
{
	p_CrossHair = std::make_shared<CrossHair>();
	p_CrossHair->Init();
}

void UIManager::End()
{
}

void UIManager::Update()
{
	p_CrossHair->Update();
}

void UIManager::Draw()
{
	p_CrossHair->Draw();
}
