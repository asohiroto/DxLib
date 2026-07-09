#include "SceneGame.h"
#include"Player.h"
#include"Camera.h"
#include"Input.h"
#include"GameDefine.h"
#include<DxLib.h>

using namespace GameDefine;

SceneGame::SceneGame() :
	p_Player(nullptr),
	p_PlayerSub(nullptr),
	p_Camera(nullptr),
	p_Input(nullptr),
	p_InputSub(nullptr)
{

}

SceneGame::~SceneGame()
{

}

void SceneGame::Init()
{
	p_Input = std::make_shared<Input>();
	p_Input->Init(DX_INPUT_PAD1);
	p_InputSub = std::make_shared<Input>();
	p_InputSub->Init(DX_INPUT_PAD2);
	p_Player = std::make_shared<Player>();
	p_Player->Init(1);
	p_PlayerSub = std::make_shared<Player>();
	p_PlayerSub->Init(2);
	p_Camera = std::make_shared<Camera>();
	p_Camera->Init();
	p_CameraSub = std::make_shared<Camera>();
	p_CameraSub->Init();
}

void SceneGame::Update()
{
	p_Input->Update();
	p_InputSub->Update();
	p_Player->Update(p_Camera->GetCameraYaw(), p_Input, p_PlayerSub);
	p_PlayerSub->Update(p_CameraSub->GetCameraYaw(), p_InputSub, p_Player);
	p_Camera->Update(p_Player, p_Input);
	p_CameraSub->Update(p_PlayerSub, p_InputSub);
}

void SceneGame::Draw()
{
	// 前フレームの描画範囲が残っている可能性があるので、まず全体に戻す
	SetDrawArea(0, 0, WIDTH, HEIGHT);

	// プレイヤー１用の画面表示処理
	p_Camera->Draw(1);
	p_Player->Draw();
	p_PlayerSub->Draw();
	DrawCircle(WIDTH / 4, HEIGHT / 2, 2, 0xffffff, true);
	DrawGrid();

	// プレイヤー２用の画面表示処理
	p_CameraSub->Draw(2);
	p_Player->Draw();
	p_PlayerSub->Draw();
	DrawCircle((WIDTH / 4) * 3, HEIGHT / 2, 2, 0xffffff, true);
	DrawGrid();


	// 二つの画面を合わせて一つの画面に表示させる
	SetDrawArea(0, 0, WIDTH, HEIGHT);

	// 仕切り線（画面中央の縦線）
	DrawLine(WIDTH / 2, 0, WIDTH / 2, HEIGHT, 0xffff00, 3);

	// 画面判別用
	DrawString(7, 10, "Player : 1", 0xffffff);
	DrawString((WIDTH / 2) + 10, 10, "Player : 2", 0xffffff);

	// デバッグ用　位置表示
	DrawFormatString(7, 30, 0xffffff, "Position X : %.2f", p_Player->GetPos().x);
	DrawFormatString(7, 50, 0xffffff, "         Y : %.2f", p_Player->GetPos().y);
	DrawFormatString(7, 70, 0xffffff, "         Z : %.2f", p_Player->GetPos().z);
	DrawFormatString((WIDTH / 2) + 10, 30, 0xffffff, "Position X : %.2f", p_PlayerSub->GetPos().x);
	DrawFormatString((WIDTH / 2) + 10, 50, 0xffffff, "         Y : %.2f", p_PlayerSub->GetPos().y);
	DrawFormatString((WIDTH / 2) + 10, 70, 0xffffff, "         Z : %.2f", p_PlayerSub->GetPos().z);

}

void SceneGame::DrawGrid() const
{
	const float lineStartX = GRID_SIZE * -(GRID_NUM * 0.5f);
	const float lineEndX = -lineStartX;

	for (int z = 0; z <= GRID_NUM; z++)
	{
		const float lineZ = GRID_SIZE * z - GRID_SIZE * GRID_NUM * 0.5f;
		DrawLine3D(VGet(lineStartX, 0, lineZ), VGet(lineEndX, 0, lineZ), 0xffffff);
	}

	const float lineStartZ = GRID_SIZE * -(GRID_NUM * 0.5f);
	const float lineEndZ = -lineStartZ;

	for (int x = 0; x <= GRID_NUM; x++)
	{
		const float lineX = GRID_SIZE * x - GRID_SIZE * GRID_NUM * 0.5f;
		DrawLine3D(VGet(lineX, 0, lineStartZ), VGet(lineX, 0, lineEndZ), 0xffffff);
	}
}