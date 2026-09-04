#include "SceneManager.h"
#include "SceneMain.h"
#include "LoadScene.h"
#include "StartScene.h"
#include "ResultScene.h"
#include "ExplainScene.h"
#include "Inputs/Input.h"
#include "GameDefine.h"
#include <DxLib.h>

using namespace GameDefine;

namespace
{
	// フェードにかけるカウンタの増加量
	constexpr int FADE_SPEED = 5;
	// フェード用アルファ値の最大値
	constexpr int ALPHA_MAX = 255;
	// 音量の最大値
	constexpr int VOLUME_MAX = 255;
	// 音量割合計算の基数
	constexpr int VOLUME_PERCENT_BASE = 100;
	// BGMの音量割合
	constexpr int BGM_VOLUME_PERCENT = 80;
	// フェード時の色
	constexpr int FADE_COLOR = 0x000000;
}

SceneManager::SceneManager() :
	p_Main(nullptr),
	p_Input(nullptr),
	p_Load(nullptr),
	p_Start(nullptr),
	p_Result(nullptr),
	_nowScene(SceneState::Load),
	_score(0),
	_defeatNum(0),
	_count(0), _gameBgm(),
	_gameSe(), _deadCount(0)
{
}

SceneManager::~SceneManager()
{
}

void SceneManager::Init()
{
	_count = 0;

	p_Main = std::make_shared<SceneMain>();
	p_Load = std::make_shared<LoadScene>();
	p_Start = std::make_shared<StartScene>();
	p_Result = std::make_shared<ResultScene>();
	p_Explain = std::make_shared<ExplainScene>();
	p_Input = std::make_shared<Input>();

	p_Load->Init();
	p_Input->Init();

	_gameBgm = p_Load->GetBGMHanadles();
	_gameSe = p_Load->GetSeHandles();

	ChangeVolumeSoundMem(VOLUME_MAX / VOLUME_PERCENT_BASE * BGM_VOLUME_PERCENT, _gameBgm.startBgmH);
	ChangeVolumeSoundMem(VOLUME_MAX / VOLUME_PERCENT_BASE * BGM_VOLUME_PERCENT, _gameBgm.explainBgmH);
	ChangeVolumeSoundMem(VOLUME_MAX / VOLUME_PERCENT_BASE * BGM_VOLUME_PERCENT, _gameBgm.mainBgmH);
	ChangeVolumeSoundMem(VOLUME_MAX / VOLUME_PERCENT_BASE * BGM_VOLUME_PERCENT, _gameBgm.resultBgmH);
}

void SceneManager::End()
{
}

void SceneManager::Update()
{
	p_Input->Update();
	_count += FADE_SPEED;

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
		{
			StopSoundMem(_gameBgm.startBgmH);
			ChangeScene(SceneManager::SceneState::Explain);
		}
		break;
	case SceneManager::SceneState::Explain:
		p_Explain->Update(p_Input);

		if (p_Explain->CanSceneChange())
		{
			StopSoundMem(_gameBgm.explainBgmH);
			ChangeScene(SceneManager::SceneState::Main);
		}
		break;
	case SceneManager::SceneState::Main:
		p_Main->Update(p_Input);

		if (p_Main->GetPlayerHp() <= 0)
		{
			StopSoundMem(_gameBgm.mainBgmH);
			ChangeScene(SceneManager::SceneState::Result);
		}
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
			StopSoundMem(_gameBgm.resultBgmH);
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
	Fade(_count);

	switch (_nowScene)
	{
	case SceneManager::SceneState::Load:
		p_Load->Draw();
		break;
	case SceneManager::SceneState::Start:
		p_Start->Draw();
		break;
	case SceneManager::SceneState::Explain:
		p_Explain->Draw();
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
	_count = 0;

	switch (_nowScene)
	{
	case SceneManager::SceneState::Load:
		p_Load->Init();
		break;
	case SceneManager::SceneState::Start:
		p_Start->Init(p_Load->GetNightDomeH());

		PlaySoundMem(_gameBgm.startBgmH, DX_PLAYTYPE_LOOP);
		break;
	case SceneManager::SceneState::Explain:
		p_Explain->Init();

		PlaySoundMem(_gameBgm.explainBgmH, DX_PLAYTYPE_LOOP);
		break;
	case SceneManager::SceneState::Main:
		p_Main->SetCharacterH(p_Load->GetPlayerH(), p_Load->GetEnemyH());
		p_Main->SetSkyDomeH(p_Load->GetNightDomeH());
		p_Main->SetOtherH(p_Load->GetHitEffectH(), p_Load->GetAtmosH());
		p_Main->SetMagics(p_Load->GetEffectHandles());
		p_Main->SetSE(_gameSe);
		p_Main->Init(_score, _defeatNum,
			p_Load->GetPlayerHpBarH(), p_Load->GetEnemyHpBarH(), p_Load->GetUltGaugeH());

		if (_defeatNum == 0)
			PlaySoundMem(_gameBgm.mainBgmH, DX_PLAYTYPE_LOOP);
		break;
	case SceneManager::SceneState::Result:
		p_Result->Init(_defeatNum, p_Load->GetSunnyDomeH());

		PlaySoundMem(_gameBgm.resultBgmH, DX_PLAYTYPE_LOOP);
		break;
	default:
		break;
	}
}

void SceneManager::Fade(int count)
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, count);
	DrawBox(0, 0, WIDTH, HEIGHT, FADE_COLOR, true);

	if (count > ALPHA_MAX)
	{
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, count);
	}
}
