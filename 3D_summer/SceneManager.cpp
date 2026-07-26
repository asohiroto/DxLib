#include "SceneManager.h"
#include "TitleScene.h"
#include "LoadScene.h"
#include "SceneGame.h"
#include "ResultScene.h"
#include "Input.h"

SceneManager::SceneManager() :
	p_Title(nullptr),
	p_Load(nullptr),
	p_Game(nullptr),
	p_Result(nullptr),
	_nowScene(SceneName::LOAD)
{
}

SceneManager::~SceneManager()
{
}

void SceneManager::Init()
{
	p_Title = std::make_shared<TitleScene>();
	p_Load = std::make_shared<LoadScene>();
	p_Game = std::make_shared<SceneGame>();
	p_Result = std::make_shared<ResultScene>();

	p_Load->Init(this);
}

void SceneManager::Update(std::shared_ptr<Input> pInput, std::shared_ptr<Input> pInputSub)
{
	if (_nowScene == TITLE) p_Title->Update(pInput);
	else if (_nowScene == LOAD) p_Load->Update();
	else if (_nowScene == GAME) p_Game->Update(pInput, pInputSub);
	else if (_nowScene == RESULT)p_Result->Update(pInput);
	else p_Load->Update();
}

void SceneManager::Draw()
{
	if (_nowScene == TITLE) p_Title->Draw();
	else if (_nowScene == LOAD) p_Load->Draw();
	else if (_nowScene == GAME) p_Game->Draw();
	else if (_nowScene == RESULT)p_Result->Draw();
	else p_Load->Draw();
}

void SceneManager::ChangeScene(int name, int winner)
{
	_nowScene = name;

	if (_nowScene == TITLE) p_Title->Init(this, p_Load->GetPlayerModelH(), p_Load->GetPlayerSubModelH(), p_Load->GetSkyDomeH());
	else if (_nowScene == LOAD) p_Load->Init(this);
	else if (_nowScene == GAME) p_Game->Init(this, p_Load->GetPlayerModelH(), p_Load->GetPlayerSubModelH(), p_Load->GetSkyDomeH());
	else if (_nowScene == RESULT)p_Result->Init(this, winner, p_Load->GetPlayerModelH(), p_Load->GetSkyDomeH());
	else p_Load->Init(this);
}