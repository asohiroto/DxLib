#pragma once
#include<vector>
#include<string>
#include"Character.h"
#include"Player.h"
#include"Enemy.h"

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
	
	std::vector<Enemy*> m_allEnemies; // すべての部隊を管理する配列

	int m_handle;
};