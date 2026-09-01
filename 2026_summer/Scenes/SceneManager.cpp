#include "SceneManager.h"
#include "SceneMain.h"
#include "LoadScene.h"
#include "StartScene.h"
#include "ResultScene.h"
#include "Inputs/Input.h"
#include <DxLib.h>

SceneManager::SceneManager() :
	p_Main(nullptr),
	p_Input(nullptr),
	p_Load(nullptr),
	p_Start(nullptr),
	p_Result(nullptr),
	_nowScene(SceneState::Load),
	_score(0),
	_defeatNum(0)
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
	p_Result = std::make_shared<ResultScene>();
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
			ChangeScene(SceneManager::SceneState::Result);
		else if (p_Main->GetEnemyHp() <= 0)
		{
			_defeatNum++;
			_score += _defeatNum;
			ChangeScene(SceneManager::SceneState::Main);
		}
		break;
	case SceneManager::SceneState::Result:
		p_Result->Update(p_Input);
		if (p_Result->CanSceneChange())
		{
			_defeatNum = 0;
			_score = 0;
			ChangeScene(SceneManager::SceneState::Start);
		}

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
	case SceneManager::SceneState::Result:
		p_Result->Draw();
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
		p_Start->Init(p_Load->GetDomeH());
		break;
	case SceneManager::SceneState::Main:
		p_Main->SetCharacterH(p_Load->GetPlayerH(), p_Load->GetEnemyH());
		p_Main->SetSkyDomeH(p_Load->GetDomeH());
		p_Main->SetOtherH(p_Load->GetHitEffectH(), p_Load->GetAtmosH());
		p_Main->SetMagics(p_Load->GetHandles());
		p_Main->Init(_score, _defeatNum);
		break;
	case SceneManager::SceneState::Result:
		p_Result->Init(_score);
		break;
	default:
		break;
	}
}
