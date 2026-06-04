#include"DXLib.h"
#include"SceneMain.h"

SceneMain::SceneMain() :
	m_moveCount(640),
	m_saikiCount(0)
{
	// ランダムに地形情報を生成
	for (int x = 0; x < kGridNumX; x++)
	{
		for (int y = 0; y < kGridNumY; y++)
		{
			int randValue = GetRand(100);

			if (randValue >= 70)
			{
				m_fieldTbl[y][x] = -1;
			}
			else if (randValue >= 50)
			{
				m_fieldTbl[y][x] = 0;
			}
			else
			{
				m_fieldTbl[y][x] = 1;
			}
		}
	}
	// ゴール位置をランダムに決定
	// ゴールが移動不可の地形に設定されると再抽選
	// 最悪永久にループするので移動負荷地形は多すぎないように配置すること
	do
	{
		m_goalX = GetRand(kGridNumX - 1);
		m_goalY = GetRand(kGridNumY - 1);
	} while (m_fieldTbl[m_goalY][m_goalX] < 0);
	// スタート位置をランダムに決定
	do
	{
		m_startX = GetRand(kGridNumX - 1);
		m_startY = GetRand(kGridNumY - 1);
	} while (m_fieldTbl[m_startY][m_startX] < 0 || m_startX != m_goalX || m_startY != m_startY  );
}

SceneMain::~SceneMain()
{
}

void SceneMain::Init()
{
}

void SceneMain::Update()
{
}

void SceneMain::Draw()
{

	// フィールドの状態を描画
	for (int x = 0; x < kGridNumX; x++)
	{
		for (int y = 0; y < kGridNumY; y++)
		{
			int posX = x * kGridSize;
			int posY = y * kGridSize;

			unsigned int color = GetColor(0, 255, 0);

			// 地形の状態によって色を変える
			if (m_fieldTbl[y][x] == -1)
			{
				color = GetColor(0, 0, 255);
			}
			else if (m_fieldTbl[y][x] == 0)
			{
				color = GetColor(255, 0, 0);
			}

			// ゴールとスタートの位置は特別な色で描画
			if (x == m_goalX && y == m_goalY)
			{
				color = GetColor(255, 255, 0);
			}
			if (x == m_startX && y == m_startY)
			{
				color = GetColor(255, 0, 255);
			}

			DrawBox(posX, posY, posX + kGridSize - 1, posY + kGridSize - 1, color, true);

		}
	}
}