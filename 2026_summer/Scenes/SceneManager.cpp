#include "SceneManager.h"
#include "SceneMain.h"

SceneManager::SceneManager():
	p_SceneMain(nullptr)
{
}

SceneManager::~SceneManager()
{
}

void SceneManager::Init()
{
	p_SceneMain = std::make_shared<SceneMain>();
	p_SceneMain->Init();
}

void SceneManager::End()
{
}

void SceneManager::Update()
{
	p_SceneMain->Update();
}

void SceneManager::Draw()
{
	p_SceneMain->Draw();
}
