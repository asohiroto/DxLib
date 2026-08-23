#include "SceneManager.h"
#include "SceneMain.h"
#include "LoadScene.h"
#include "Inputs/Input.h"

SceneManager::SceneManager() :
	p_SceneMain(nullptr),
	p_Input(nullptr),
	p_LoadScene(nullptr),
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
	p_Input = std::make_shared<Input>();

	p_LoadScene->Init();
	p_Input->Init();
}

void SceneManager::End()
{
}

void SceneManager::Update()
{
	p_Input->Update();

	switch (_nowScene)
	{
	case SceneManager::SceneState::Load:
		p_LoadScene->Update(p_Input);

		if (p_LoadScene->CanSceneChange())
			ChangeScene(SceneManager::SceneState::Game);

		break;

	case SceneManager::SceneState::Start:

		break;

	case SceneManager::SceneState::Game:
		p_SceneMain->Update(p_Input);

		if (p_SceneMain->GetPlayerHp() <= 0)
			ChangeScene(SceneManager::SceneState::GameOver);
		else if (p_SceneMain->GetEnemyHp() <= 0)
			ChangeScene(SceneManager::SceneState::Clear);

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
	p_Input->Draw();

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
		p_SceneMain->SetCharacterH(p_LoadScene->GetPlayerH(), p_LoadScene->GetEnemyH());
		p_SceneMain->SetSkyDomeH(p_LoadScene->GetDomeH());
		p_SceneMain->SetMagicH(p_LoadScene->GetMagicShotH(), p_LoadScene->GetMissileH(), p_LoadScene->GetHitEffectH());
		p_SceneMain->Init();
		break;
	case SceneManager::SceneState::Clear:
		break;
	case SceneManager::SceneState::GameOver:
		break;
	default:
		break;
	}
}
