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

	_Load.Init();
	_Input.Init();

	_gameBgm = _Load.GetBGMHanadles();
	_gameSe = _Load.GetSeHandles();

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
	_Input.Update();
	_count += FADE_SPEED;

	switch (_nowScene)
	{
	case SceneManager::SceneState::Load:
		_Load.Update(_Input);

		if (_Load.CanSceneChange())
			ChangeScene(SceneManager::SceneState::Start);
		break;
	case SceneManager::SceneState::Start:
		_Start.Update(_Input);

		if (_Start.CanSceneChange())
		{
			StopSoundMem(_gameBgm.startBgmH);
			ChangeScene(SceneManager::SceneState::Explain);
		}
		break;
	case SceneManager::SceneState::Explain:
		_Explain.Update(_Input);

		if (_Explain.CanSceneChange())
		{
			StopSoundMem(_gameBgm.explainBgmH);
			ChangeScene(SceneManager::SceneState::Main);
		}
		break;
	case SceneManager::SceneState::Main:
		_Main.Update(_Input);

		if (_Main.GetPlayerHp() <= 0)
		{
			StopSoundMem(_gameBgm.mainBgmH);
			ChangeScene(SceneManager::SceneState::Result);
		}
		else if (_Main.GetEnemyHp() <= 0)
		{
			// 倒した数に応じてスコアを加算し、Mainシーンを再初期化してリポップさせる
			_defeatNum++;
			_score += _defeatNum;
			ChangeScene(SceneManager::SceneState::Main);
		}
		break;
	case SceneManager::SceneState::Result:
		_Result.Update(_Input);

		if (_Result.CanSceneChange())
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
	_Input.Draw();
	Fade(_count);

	switch (_nowScene)
	{
	case SceneManager::SceneState::Load:
		_Load.Draw();
		break;
	case SceneManager::SceneState::Start:
		_Start.Draw();
		break;
	case SceneManager::SceneState::Explain:
		_Explain.Draw();
		break;
	case SceneManager::SceneState::Main:
		_Main.Draw();
		break;
	case SceneManager::SceneState::Result:
		_Result.Draw();
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
		_Load.Init();
		break;
	case SceneManager::SceneState::Start:
		_Start.Init(_Load.GetNightDomeH());

		PlaySoundMem(_gameBgm.startBgmH, DX_PLAYTYPE_LOOP);
		break;
	case SceneManager::SceneState::Explain:
		_Explain.Init();

		PlaySoundMem(_gameBgm.explainBgmH, DX_PLAYTYPE_LOOP);
		break;
	case SceneManager::SceneState::Main:
		_Main.SetCharacterH(_Load.GetPlayerH(), _Load.GetEnemyH());
		_Main.SetSkyDomeH(_Load.GetNightDomeH());
		_Main.SetOtherH(_Load.GetHitEffectH(), _Load.GetAtmosH());
		_Main.SetMagics(_Load.GetEffectHandles());
		_Main.SetSE(_gameSe);
		_Main.Init(_score, _defeatNum,
			_Load.GetPlayerHpBarH(), _Load.GetEnemyHpBarH(), _Load.GetUltGaugeH());

		// リポップ時にBGMが最初から再生し直されないよう、初回のみ再生する
		if (_defeatNum == 0)
			PlaySoundMem(_gameBgm.mainBgmH, DX_PLAYTYPE_LOOP);
		break;
	case SceneManager::SceneState::Result:
		_Result.Init(_defeatNum, _Load.GetSunnyDomeH());

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
