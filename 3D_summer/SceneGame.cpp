#define NOMINMAX
#include "SceneGame.h"
#include"Player.h"
#include"Camera.h"
#include"Input.h"
#include"UIManager.h"
#include"GameDefine.h"
#include<algorithm>
#include<DxLib.h>

using namespace GameDefine;

SceneGame::SceneGame() :
	p_Player(nullptr),
	p_PlayerSub(nullptr),
	p_Camera(nullptr),
	p_Input(nullptr),
	p_InputSub(nullptr),
	_hitstopCount(0),
	p_UIManager(nullptr),
	_zoomDistance(0)
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
	p_UIManager = std::make_shared<UIManager>();

	_zoomDistance = CAMERA_DISTANCE;
}

void SceneGame::Update()
{
	p_Input->Update();
	p_InputSub->Update();

	p_Camera->Update(p_Player, p_PlayerSub, p_Input);
	p_CameraSub->Update(p_PlayerSub, p_Player, p_InputSub);

	// ヒットストップする場合は、ここをループ
	if (_hitstopCount > 0)
	{
		_hitstopCount--;
		_zoomDistance += (CAMERA_HITSTOP_ZOOM - _zoomDistance) * ZOOM_LERP_RATE;
		p_Camera->SetCameraDistance(_zoomDistance);
		return;
	}

	_zoomDistance += (CAMERA_DISTANCE - _zoomDistance) * ZOOM_LERP_RATE;
	p_Camera->SetCameraDistance(_zoomDistance);

	p_Player->Update(p_Camera->GetCameraYaw(), p_Input, p_PlayerSub);
	p_PlayerSub->Update(p_CameraSub->GetCameraYaw(), p_InputSub, p_Player);
	p_UIManager->Update(p_Player, p_PlayerSub);

	// 両プレイヤーからヒットストップのフレームを取得
	int request1 = p_Player->HitstopRequest();
	int request2 = p_PlayerSub->HitstopRequest();

	_hitstopCount = std::max(request1, request2);

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
	// ステージの床を描画
	DrawCube3D
	(
		VGet(GRID_NUM / 2 * GRID_SIZE, 0, GRID_NUM / 2 * GRID_SIZE),
		VGet(-(GRID_NUM / 2 * GRID_SIZE), -20.0f, -(GRID_NUM / 2 * GRID_SIZE)),
		0xffffff, 0xffffff, true
	);

#ifdef _DEBUG
	DrawGrid();
#endif

	// プレイヤー２用の画面表示処理
	p_CameraSub->Draw(2);
	p_Player->Draw();
	p_PlayerSub->Draw();
	DrawCircle((WIDTH / 4) * 3, HEIGHT / 2, 2, 0xffffff, true);
	// ステージの床を描画
	DrawCube3D
	(
		VGet(GRID_NUM / 2 * GRID_SIZE, 0, GRID_NUM / 2 * GRID_SIZE),
		VGet(-(GRID_NUM / 2 * GRID_SIZE), -20.0f, -(GRID_NUM / 2 * GRID_SIZE)),
		0xffffff, 0xffffff, true
	);

#ifdef _DEBUG
	DrawGrid();
#endif

	// 二つの画面を合わせて一つの画面に表示させる
	SetDrawArea(0, 0, WIDTH, HEIGHT);

	// 仕切り線（画面中央の縦線）
	DrawLine(WIDTH / 2, 0, WIDTH / 2, HEIGHT, 0xffff00, 3);

#ifdef _DEBUG
	// 画面判別用
	DrawString(7, 10, "Player : 1", 0xffffff);
	DrawString((WIDTH / 2) + 10, 10, "Player : 2", 0xffffff);

	// デバッグ用　位置表示
	DrawFormatString(7, 30, 0xffffff, "Position X : %.2f", p_Player->GetPos().x);
	DrawFormatString(7, 50, 0xffffff, "         Y : %.2f", p_Player->GetPos().y);
	DrawFormatString(7, 70, 0xffffff, "         Z : %.2f", p_Player->GetPos().z);
	DrawFormatString(7, 90, 0xffffff, "HP : %d / %d", p_Player->GetHp(), p_Player->GetMaxHp());
	DrawFormatString((WIDTH / 2) + 10, 30, 0xffffff, "Position X : %.2f", p_PlayerSub->GetPos().x);
	DrawFormatString((WIDTH / 2) + 10, 50, 0xffffff, "         Y : %.2f", p_PlayerSub->GetPos().y);
	DrawFormatString((WIDTH / 2) + 10, 70, 0xffffff, "         Z : %.2f", p_PlayerSub->GetPos().z);
	DrawFormatString((WIDTH / 2) + 10, 90, 0xffffff, "HP : %d / %d", p_PlayerSub->GetHp(), p_PlayerSub->GetMaxHp());
#endif

	p_UIManager->Draw(p_Player, p_PlayerSub);
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