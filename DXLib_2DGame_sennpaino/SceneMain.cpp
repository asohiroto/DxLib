#include"DxLib.h"
#include "SceneMain.h"
#include"Player.h"
#include"Enemy.h"

SceneMain::SceneMain()
{
	m_pPlayer = new Player;

	m_handle = LoadGraph("media/bg.png");
}

SceneMain::~SceneMain()
{
	delete m_pPlayer;
	for (Enemy* e : m_allEnemies)
	{
		delete e;
	}
	DeleteGraph(m_handle);
}

void SceneMain::Init()
{
	m_pPlayer->Init();

	for (Enemy* e : m_allEnemies) { delete e;}
	m_allEnemies.clear();

	for (int i = 0; i < 5; i++)
	{
		Enemy* pNewEnemy = new Enemy();
		pNewEnemy->Init(m_pPlayer);
		pNewEnemy->m_pos = Vec2(200.0f + (i * 150.0f), 100.0f);
		m_allEnemies.push_back(pNewEnemy);
	}
}

void SceneMain::Update()
{
	m_pPlayer->Update();
	
	for (Enemy* e : m_allEnemies)
	{
		e->Update();
	}
}

void SceneMain::Draw()
{
	DrawGraph(0, -200, m_handle, TRUE);

	m_pPlayer->Draw();
	
	for (Enemy* e : m_allEnemies)
	{
		e->Draw();
	}
}
