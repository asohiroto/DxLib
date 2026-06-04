#pragma once
#include<vector>
#include<string>
#include"Character.h"
#include"Player.h"
#include"Enemy.h"
#include"Castle.h"

class Player;
class Enemy;

/// <summary>
/// シーン管理クラス
/// </summary>
class SceneMain
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	SceneMain();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~SceneMain();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private:
	Player* m_pPlayer;
	Castle* m_pCastle;
	
	// すべての部隊を管理する配列
	std::vector<Enemy*> m_allEnemies;

	// 勝利可能フラグ
	bool victoryFlag;

	int m_handle;
	int enemyAmount;
	int allEnemySoldier;
	int allPlayerSoldier;
};