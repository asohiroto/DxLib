#include "SceneManager.h"
#include "SceneMain.h"
#include "LoadScene.h"

SceneManager::SceneManager() :
	p_SceneMain(nullptr),
	_nowScene(SceneState::Load)
{
}

SceneManager::~SceneManager()
{
}

void SceneManager::Init()
{
	p_SceneMain = std::make_shared<SceneMain>();
	p_LoadScene = std::make_shared<LoadScene>();

	p_LoadScene->Init();
}

void SceneManager::End()
{
}

void SceneManager::Update()
{
	if (p_LoadScene->CanSceneChange())
		ChangeScene(SceneManager::SceneState::Game);

	switch (_nowScene)
	{
	case SceneManager::SceneState::Load:
		p_LoadScene->Update();
		break;
	case SceneManager::SceneState::Start:
		break;
	case SceneManager::SceneState::Game:
		p_SceneMain->Update();
		break;
	case SceneManager::SceneState::Clear:
		break;
	case SceneManager::SceneState::GameOver:
		break;
	default:
		break;
	}
}

void SceneManager::Draw()
{
	switch (_nowScene)
	{
	case SceneManager::SceneState::Load:
		p_LoadScene->Draw();
		break;
	case SceneManager::SceneState::Start:
		break;
	case SceneManager::SceneState::Game:
		p_SceneMain->Draw();
		break;
	case SceneManager::SceneState::Clear:
		break;
	case SceneManager::SceneState::GameOver:
		break;
	default:
		break;
	}
}

void SceneManager::ChangeScene(SceneState nextScene)
{
	_nowScene = nextScene;

	switch (_nowScene)
	{
	case SceneManager::SceneState::Load:
		p_LoadScene->Init();
		break;
	case SceneManager::SceneState::Start:
		break;
	case SceneManager::SceneState::Game:
		p_SceneMain->Init(p_LoadScene->GetPlayerH(), p_LoadScene->GetEnemyH(), p_LoadScene->GetDomeH());
		break;
	case SceneManager::SceneState::Clear:
		break;
	case SceneManager::SceneState::GameOver:
		break;
	default:
		break;
	}
}
