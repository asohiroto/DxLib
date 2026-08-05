#include "SceneMain.h"
#include "Players/Player.h"
#include "Cameras/Camera.h"
#include "Inputs/Input.h"
#include "Enemys/Enemy.h"
#include "Colls/Collision.h"
#include "Magics/MagicManager.h"
#include "Colls/MagicCollision.h"
#include "UIs/UIManager.h"

namespace
{
	// 床に描画するグリッドのサイズ
	constexpr float GRID_SIZE = 100;
	// 床に描画するグリッドの数
	constexpr float GRID_NUM = 20;
}

SceneMain::SceneMain() :
	p_Player(nullptr),
	p_Camera(nullptr),
	p_Input(nullptr),
	p_Enemy(nullptr),
	p_Coll(nullptr),
	p_MManager(nullptr),
	p_MColl(nullptr),
	p_UI(nullptr)
{
}

SceneMain::~SceneMain()
{
}

void SceneMain::Init()
{
	p_Player = std::make_shared<Player>();
	p_Camera = std::make_shared<Camera>();
	p_Input = std::make_shared<Input>();
	p_Enemy = std::make_shared<Enemy>();
	p_Coll = std::make_shared<Collision>();
	p_MManager = std::make_shared<MagicManager>();
	p_MColl = std::make_shared<MagicCollision>();
	p_UI = std::make_shared<UIManager>();

	p_Player->Init();
	p_Camera->Init();
	p_Input->Init();
	p_Enemy->Init();
	p_MManager->Init();
	p_UI->Init();
}

void SceneMain::End()
{

}

void SceneMain::Update()
{
	p_Player->Update(p_Input, p_Camera, p_MManager);
	p_Camera->Update(p_Player, p_Enemy, p_Input);
	p_Input->Update();
	p_Enemy->Update();
	p_Coll->Update(p_Player, p_Enemy);
	p_MManager->Update(p_Player->GetPos(), p_Enemy->GetPos());
	p_MColl->Update(p_Player, p_Enemy, p_MManager->GetPlayerList(), p_MManager->GetEnemyList());
	p_MManager->RemoveList();
	//p_UI->Update();
}

void SceneMain::Draw()
{
	p_Player->Draw();
	p_Camera->Draw();
	p_Input->Draw();
	p_Enemy->Draw();
	p_MManager->Draw();
	//p_UI->Draw();

	DrawGrid();
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