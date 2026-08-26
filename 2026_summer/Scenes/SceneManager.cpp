#include "SceneManager.h"
#include "SceneMain.h"
#include "LoadScene.h"
#include "StartScene.h"
#include "ClearScene.h"
#include "GameOverScene.h"
#include "Inputs/Input.h"
#include <DxLib.h>

SceneManager::SceneManager() :
	p_Main(nullptr),
	p_Input(nullptr),
	p_Load(nullptr),
	p_Start(nullptr),
	p_Clear(nullptr),
	p_GameOver(nullptr),
	_nowScene(SceneState::Load)
{
}

SceneManager::~SceneManager()
{
}

void SceneManager::Init()
{
	p_Main = std::make_shared<SceneMain>();
	p_Load = std::make_shared<LoadScene>();
	p_Start = std::make_shared<StartScene>();
	p_Clear = std::make_shared<ClearScene>();
	p_GameOver = std::make_shared<GameOverScene>();
	p_Input = std::make_shared<Input>();

	p_Load->Init();
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
		p_Load->Update(p_Input);

		if (p_Load->CanSceneChange())
			ChangeScene(SceneManager::SceneState::Start);

		break;

	case SceneManager::SceneState::Start:
		p_Start->Update(p_Input);

		if (p_Start->CanSceneChange())
			ChangeScene(SceneManager::SceneState::Main);
		break;

	case SceneManager::SceneState::Main:
		p_Main->Update(p_Input);

		if (p_Main->GetPlayerHp() <= 0)
			ChangeScene(SceneManager::SceneState::GameOver);
		else if (p_Main->GetEnemyHp() <= 0)
			ChangeScene(SceneManager::SceneState::Clear);

		break;

	case SceneManager::SceneState::Clear:
		p_Clear->Update(p_Input);

		if (p_Clear->CanSceneChange())
			ChangeScene(SceneManager::SceneState::Start);
		break;

	case SceneManager::SceneState::GameOver:
		p_GameOver->Update(p_Input);

		if (p_GameOver->CanSceneChange())
			ChangeScene(SceneManager::SceneState::Start);

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
		p_Load->Draw();
		break;
	case SceneManager::SceneState::Start:
		p_Start->Draw();
		break;
	case SceneManager::SceneState::Main:
		p_Main->Draw();
		break;
	case SceneManager::SceneState::Clear:
		p_Clear->Draw();
		break;
	case SceneManager::SceneState::GameOver:
		p_GameOver->Draw();
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
		p_Load->Init();
		break;
	case SceneManager::SceneState::Start:
		p_Start->Init();
		break;
	case SceneManager::SceneState::Main:
		p_Main->SetCharacterH(p_Load->GetPlayerH(), p_Load->GetEnemyH());
		p_Main->SetSkyDomeH(p_Load->GetDomeH());
		p_Main->SetPlayerMagicH(p_Load->GetMagicShotH(), p_Load->GetMissileH(), p_Load->GetFuryH());
		p_Main->SetEnemyMagicH(p_Load->GetBeamH());
		p_Main->SetOtherH(p_Load->GetHitEffectH(), p_Load->GetCircleEffectH(), p_Load->GetAtmosH());
		p_Main->Init();
		break;
	case SceneManager::SceneState::Clear:
		p_Clear->Init();
		break;
	case SceneManager::SceneState::GameOver:
		p_GameOver->Init();
		break;
	default:
		break;
	}
}
