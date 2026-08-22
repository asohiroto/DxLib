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
	a(-1),
	b(-1),
	c(-1)
{
}

SceneMain::~SceneMain()
{
}

void SceneMain::Init(int playerH, int enemyH, int domeH)
{
	p_Player = std::make_shared<Player>();
	p_Camera = std::make_shared<Camera>();
	p_EManager = std::make_shared<EnemyManager>();
	p_Coll = std::make_shared<Collision>();
	p_MManager = std::make_shared<MagicManager>();
	p_MColl = std::make_shared<MagicCollision>();
	p_UI = std::make_shared<UIManager>();
	p_Dome = std::make_unique<SkyDome>();
	p_EffectManager = std::make_shared<EffectManager>();

	p_Player->Init(playerH);
	p_Camera->Init();
	p_EManager->Init(enemyH);
	p_MManager->Init();
	p_UI->Init(p_EManager->GetMaxHp(), p_Player->GetMaxHp());
	p_Dome->Init(domeH);

	p_EffectManager->Init();
	p_EffectManager->Load();

	a = playerH;
	b = enemyH;
	c = domeH;
}

void SceneMain::End()
{

}

void SceneMain::Update(std::shared_ptr<Input> pInput)
{
	p_Dome->Update();
	p_Player->Update(pInput, p_Camera, p_MManager);
	p_Camera->Update(p_Player, p_EManager->GetEnemyPointer(), pInput);
	p_EManager->Update(p_Player->GetPos(), p_MManager);
	p_Coll->Update(p_Player, p_EManager->GetEnemyPointer());
	p_MManager->Update(p_Player->GetPos(), p_EManager->GetEnemyPos());
	p_MColl->Update(p_Player, p_EManager->GetEnemyPointer(), p_MManager->GetPlayerList(), p_MManager->GetEnemyList());
	p_MManager->RemoveList();
	p_UI->Update(p_EManager->GetNowHp(), p_Player->GetNowHp());

	p_EffectManager->Update();
}

void SceneMain::Draw()
{
#ifdef _DEBUG
	DrawGrid();

	// ステージの床を描画
	DrawCube3D
	(
		VGet(-(GRID_NUM / 2 * GRID_SIZE), -20.0f, -(GRID_NUM / 2 * GRID_SIZE)),
		VGet(GRID_NUM / 2 * GRID_SIZE, 0, GRID_NUM / 2 * GRID_SIZE),
		0xffffff, 0xffffff, true
	);

#endif

	p_Dome->Draw();
	p_Player->Draw();
	p_Camera->Draw();
	p_EManager->Draw();
	p_MManager->Draw();
	p_EffectManager->Draw();
	p_UI->Draw();

#ifdef _DEBUG
	DrawFormatString(800, 450, 0x000000, "%d, %d, %d", a, b, c);
#endif
}

void SceneMain::DrawGrid()
{
	float lineStartX = GRID_SIZE * -(GRID_NUM * 0.5f);
	float lineEndX = -lineStartX;

	for (int z = 0; z <= GRID_NUM; z++)
	{
		float lineZ = GRID_SIZE * z - GRID_SIZE * GRID_NUM * 0.5f;
		DrawLine3D(VGet(lineStartX, 0, lineZ), VGet(lineEndX, 0, lineZ), 0xffffff);
	}

	float lineStartZ = GRID_SIZE * -(GRID_NUM * 0.5f);
	float lineEndZ = -lineStartZ;

	for (int x = 0; x <= GRID_NUM; x++)
	{
		float lineX = GRID_SIZE * x - GRID_SIZE * GRID_NUM * 0.5f;
		DrawLine3D(VGet(lineX, 0, lineStartZ), VGet(lineX, 0, lineEndZ), 0xffffff);
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
