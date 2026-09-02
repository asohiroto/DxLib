#include "SceneMain.h"
#include "Players/Player.h"
#include "Cameras/Camera.h"
#include "Enemys/EnemyManager.h"
#include "Colls/Collision.h"
#include "Magics/MagicManager.h"
#include "Colls/MagicCollision.h"
#include "UIs/UIManager.h"
#include "SkyDome.h"
#include "Effects/EffectManager.h"

namespace
{
	// 床に描画するグリッドのサイズ
	constexpr float GRID_SIZE = 100;
	// 床に描画するグリッドの数
	constexpr float GRID_NUM = 60;
	// リポップするまでのフレーム数
	constexpr int REPOP_FRAME = 60;
}

SceneMain::SceneMain() :
	p_Player(nullptr),
	p_Camera(nullptr),
	p_EManager(nullptr),
	p_Coll(nullptr),
	p_MManager(nullptr),
	p_MColl(nullptr),
	p_UI(nullptr),
	p_Dome(nullptr),
	p_EffectManager(nullptr),
	_playerTempH(-1), _enemyTempH(-1),
	_playerMagicsTemp(),
	_domeTempH(-1),
	_hitTempH(-1),
	_atmosH(-1), _atmosPlayingH(-1), _atmosCount(0),
	_hitStopCount(0), _score(0), _repopCount(0), _killCount(0)
{
}

SceneMain::~SceneMain()
{
}

void SceneMain::Init(int score, int killCount, int pHpH, int eHpH, int ultH)
{
	_score = score;
	_killCount = killCount;

	p_Player = std::make_shared<Player>();
	p_Camera = std::make_shared<Camera>();
	p_EManager = std::make_shared<EnemyManager>();
	p_Coll = std::make_shared<Collision>();
	p_MManager = std::make_shared<MagicManager>();
	p_MColl = std::make_shared<MagicCollision>();
	p_UI = std::make_shared<UIManager>();
	p_Dome = std::make_unique<SkyDome>();
	p_EffectManager = std::make_shared<EffectManager>();

	p_Player->Init(_playerTempH, _playerMagicsTemp);
	p_Camera->Init();
	p_EManager->Init(_enemyTempH, _playerMagicsTemp, _score);
	p_MManager->Init();
	p_UI->Init(p_EManager->GetMaxHp(), p_Player->GetMaxHp(),
		p_Player->GetMaxMp(), p_Player->GetMaxCharge(),
		pHpH, eHpH, ultH);
	p_Dome->Init(_domeTempH);
	p_Coll->Init();
	p_MColl->Init(_hitTempH, _score);

	p_EffectManager->Init();
	p_EffectManager->Load();

	_repopCount = REPOP_FRAME;
}

void SceneMain::End()
{

}

void SceneMain::Update(std::shared_ptr<Input> pInput)
{
	_atmosCount++;

	p_Camera->Update(p_Player, p_EManager->GetEnemyPointer(), pInput);

	if (_repopCount > 0)
	{
		p_Camera->SetCameraMode(true);
		p_Player->SetRotate(p_Camera->GetCameraYaw());
		_repopCount--;
		if (_repopCount == 0) p_Camera->SetCameraMode(false);
		return;
	}

	if (_hitStopCount > 0)
	{
		_hitStopCount--;
		return;
	}

	p_Dome->Update();
	p_Player->Update(pInput, p_Camera, p_MManager);
	p_EManager->Update(p_Player->GetPos(), p_MManager);
	p_Coll->Update(p_Player, p_EManager->GetEnemyPointer());
	p_MManager->Update(p_Player->GetPos(), p_EManager->GetEnemyPos());
	p_MColl->Update(p_Player, p_EManager->GetEnemyPointer(), p_MManager->GetPlayerList(), p_MManager->GetEnemyList());
	p_MManager->RemoveList();

	p_UI->Update(p_EManager->GetNowHp(), p_Player->GetNowHp(),
		p_Player->GetNowMp(), p_Player->GetNowCharge());

	p_EffectManager->Update();

	_hitStopCount = p_MColl->GetHitStopFrame();

	if (_atmosCount % 240 == 0)
		_atmosPlayingH = PlayEffekseer3DEffect(_atmosH);

	if (_atmosCount % 240 == 239)
		StopEffekseer3DEffect(_atmosPlayingH);
}

void SceneMain::Draw()
{
	p_Camera->Draw();

	DrawStage();

	p_Dome->Draw();
	p_Player->Draw();
	p_EManager->Draw();
	p_MManager->Draw();
	p_EffectManager->Draw();
	p_UI->Draw();

	if (_repopCount > 0)
	{
		DrawBox(0, 200, 1600, 800, 0x000000, true);

		SetFontSize(400);
		DrawFormatString(0, 300, 0xffffff, "%d 人目!!", _killCount + 1);
		SetFontSize(20);
	}
	else
	{
		SetFontSize(70);
		DrawFormatString(975, 35, 0xff0000, "%d 人目...", _killCount + 1);
		DrawFormatString(970, 30, 0xffffff, "%d 人目...", _killCount + 1);
		SetFontSize(20);
	}


#ifdef _DEBUG
	DrawFormatString(1300, 300, 0x000000, "score : %d", _score);
#endif
}

void SceneMain::DrawStage()
{
	// ステージの床を描画
	DrawCube3D
	(
		VGet(-(GRID_NUM / 2 * GRID_SIZE), -20.0f, -(GRID_NUM / 2 * GRID_SIZE)),
		VGet(GRID_NUM / 2 * GRID_SIZE, 0, GRID_NUM / 2 * GRID_SIZE),
		0xffffff, 0xffffff, true
	);

	float lineStartX = GRID_SIZE * -(GRID_NUM * 0.5f);
	float lineEndX = -lineStartX;

	for (int z = 0; z <= GRID_NUM; z++)
	{
		float lineZ = GRID_SIZE * z - GRID_SIZE * GRID_NUM * 0.5f;
		DrawLine3D(VGet(lineStartX, 0, lineZ), VGet(lineEndX, 0, lineZ), 0x000000);
	}

	float lineStartZ = GRID_SIZE * -(GRID_NUM * 0.5f);
	float lineEndZ = -lineStartZ;

	for (int x = 0; x <= GRID_NUM; x++)
	{
		float lineX = GRID_SIZE * x - GRID_SIZE * GRID_NUM * 0.5f;
		DrawLine3D(VGet(lineX, 0, lineStartZ), VGet(lineX, 0, lineEndZ), 0x00000);
	}
}

int SceneMain::GetPlayerHp() const
{
	return p_Player->GetNowHp();
}

int SceneMain::GetEnemyHp() const
{
	return p_EManager->GetNowHp();
}
