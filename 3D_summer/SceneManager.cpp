#include "SceneManager.h"
#include "TitleScene.h"
#include "SceneGame.h"

SceneManager::SceneManager() :
	p_Title(nullptr),
	p_Game(nullptr),
	_nowScene(SceneName::TITLE)
{
}

SceneManager::~SceneManager()
{
}

void SceneManager::Init()
{
	p_Title = std::make_shared<TitleScene>();
	p_Game = std::make_shared<SceneGame>();

	p_Title->Init(this);
}

void SceneManager::Update()
{
	if (_nowScene == TITLE) p_Title->Update();
	else if (_nowScene == GAME) p_Game->Update();
	else p_Title->Update();
}

void SceneManager::Draw()
{
	if (_nowScene == TITLE) p_Title->Draw();
	else if (_nowScene == GAME) p_Game->Draw();
	else p_Title->Draw();
}

void SceneManager::ChangeScene(int name)
{
	_nowScene = name;

	if (_nowScene == TITLE) p_Title->Init(this);
	else if (_nowScene == GAME) p_Game->Init(this);
	else p_Title->Init(this);
}