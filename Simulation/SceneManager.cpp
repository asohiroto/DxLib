#include "SceneManager.h"
#include"SceneBase.h"
#include"StartScene.h"

SceneManager::SceneManager() :
	_nextScene(nullptr)
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
	if (_nowScene.empty() && _nextScene)
	{
		_nowScene.emplace_back(_nextScene);
		_nowScene.back()->Init();
		_nextScene = nullptr;
	}

	if (!_nowScene.empty())
	{
		_nowScene.back()->Update();
	}

	if (_nextScene)
	{
		if (!_nowScene.empty())
		{
			_nowScene.back()->End();
			_nowScene.back() = _nextScene;
		}
		else
		{
			_nowScene.emplace_back(_nextScene);
		}

		_nowScene.back()->Init();
		_nextScene = nullptr;
	}
}

void SceneManager::Draw()
{
	_nowScene.back()->Draw();
}

void SceneManager::ChangeScene(std::shared_ptr<SceneBase> scene)
{

	_nextScene = scene;
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