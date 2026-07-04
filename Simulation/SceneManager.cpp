#include "SceneManager.h"
#include"SceneBase.h"
#include"StartScene.h"

SceneManager::SceneManager()
{
	ChangeScene(std::make_shared<StartScene>(*this));
}

SceneManager::~SceneManager()
{

}

void SceneManager::Init()
{
}

void SceneManager::Update()
{
	_nowScene.back()->Update();
}

void SceneManager::Draw()
{
	_nowScene.back()->Draw();
}

void SceneManager::ChangeScene(std::shared_ptr<SceneBase> scene)
{
	if (_nowScene.empty())
	{
		_nowScene.emplace_back(scene);
	}
	else
	{
		_nowScene.back()->End();
		_nowScene.back() = scene;
	}

	_nowScene.back()->Init();
}

void SceneManager::PushScene(std::shared_ptr<SceneBase> scene)
{
	_nowScene.emplace_back(scene);
	_nowScene.back()->Init();
}

void SceneManager::PopScene(std::shared_ptr<SceneBase> scene)
{
	if (_nowScene.size() <= 1) return;
	_nowScene.back()->End();
	_nowScene.pop_back();
}