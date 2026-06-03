#include"DxLib.h"
#include "SceneMain.h"
#include"Player.h"
#include"Enemy.h"

SceneMain::SceneMain()
{
	m_pPlayer = new Player;

	// 背景画像の読み込み
	m_handle = LoadGraph("media/bg.png");
}

SceneMain::~SceneMain()
{
	// プレイヤーのメモリ開放
	delete m_pPlayer;

	// 配列に残っているすべての敵のメモリを解放
	for (Enemy* e : m_allEnemies)
	{
		delete e;
	}

	// 背景画像のグラフィックハンドルを削除
	DeleteGraph(m_handle);
}

void SceneMain::Init()
{
	m_pPlayer->Init();

	enemyAmount = 5; // 生成する敵の数

	// すでに戦場にいる敵をすべて削除する
	for (Enemy* e : m_allEnemies) { delete e; }
	m_allEnemies.clear();

	// 敵部隊を作成
	for (int i = 0; i < enemyAmount; i++)
	{
		Enemy* pNewEnemy = new Enemy();

		// プレイヤーのポインタと、タイプを渡して初期化
		pNewEnemy->Init(m_pPlayer, i % 3);

		// 敵が等間隔に並ぶように初期位置を設定
		pNewEnemy->m_pos = Vec2(200.0f + (i * 150.0f), 100.0f);

		// 管理用配列に登録
		m_allEnemies.push_back(pNewEnemy);
	}
}

void SceneMain::Update()
{
	// プレイヤーの移動や更新
	m_pPlayer->Update();

	// 敵部隊すべての移動や更新
	for (Enemy* e : m_allEnemies)
	{
		e->Update();
	}

	// 敵同士の重なりを解消する処理
	for (size_t i = 0; i < m_allEnemies.size(); i++)
	{
		for (size_t j = i + 1; j < m_allEnemies.size(); j++)
		{
			Enemy* enemyA = m_allEnemies[i];
			Enemy* enemyB = m_allEnemies[j];

			// ２部隊間の距離を計算
			Vec2 between = enemyB->m_pos - enemyA->m_pos;
			float distance = between.length();

			// 衝突する基準の距離
			float colRadius = 32.0f;
			float minDistance = colRadius * 2.0f;

			if (distance < minDistance)
			{
				// 完全に同じ座標ならスキップする（0で割らないように）
				if (distance == 0.0f) continue;

				// めり込んでいる深さ
				float overlap = minDistance - distance;

				// 押し出す方向ベクトル
				Vec2 pushDir = between.getNormalize();

				// お互いを半分ずつ反対方向へ押し出す
				enemyA->m_pos -= pushDir * (overlap * 0.5f);
				enemyB->m_pos += pushDir * (overlap * 0.5f);

			}
		}

		// プレイヤーと敵全員の当たり判定
		for (Enemy* e : m_allEnemies)
		{
			// プレイヤーと敵の距離を計算
			Vec2 between = e->m_pos - m_pPlayer->m_pos;
			float distance = between.length();

			// 接触判定距離
			float minDistance = 64.0f;

			// 十分に接近していて、完全に同じ座標でなければ
			if (distance < minDistance && distance > 0.0f)
			{
				// めり込んでいる深さ
				float overlap = minDistance - distance;

				// プレイヤーから見た敵の方向
				Vec2 pushDir = between.getNormalize();

				// それぞれの部隊データを呼び出す
				UnitData& enemyData = e->GetUnitData();
				UnitData& playerData = m_pPlayer->GetUnitData();

				// お互いの攻撃力の２倍、兵力を減らす
				playerData.soldierCount -= enemyData.attack * 2;
				enemyData.soldierCount -= playerData.attack * 2;

				// 押し戻し
				e->m_pos += pushDir * overlap;
				
			}
		}

		// 攻撃範囲と敵全員のダメージ処理
		for (Enemy* e : m_allEnemies)
		{
			// プレイヤーと敵の距離を計算
			Vec2 between = e->m_pos - m_pPlayer->attackPos;
			float distance = between.length();

			// それぞれの部隊データを呼び出す
			UnitData& enemyData = e->GetUnitData();
			UnitData& playerData = m_pPlayer->GetUnitData();

			// 攻撃範囲内にいれば
			if (distance < playerData.attackDistance)
			{
				
				// プレイヤーから見た敵の方向
				Vec2 pushDir = between.getNormalize();

				// 攻撃力分、兵力を減らす
				enemyData.soldierCount -= playerData.attack;
			}
		}

		// 攻撃範囲と敵全員のダメージ処理
		for (Enemy* e : m_allEnemies)
		{
			// プレイヤーと敵の距離を計算
			Vec2 between = e->attackPos - m_pPlayer->m_pos;
			float distance = between.length();

			// それぞれの部隊データを呼び出す
			UnitData& enemyData = e->GetUnitData();
			UnitData& playerData = m_pPlayer->GetUnitData();

			// 攻撃範囲内にいれば
			if (distance < enemyData.attackDistance)
			{
				// プレイヤーから見た敵の方向
				Vec2 pushDir = between.getNormalize();

				// 攻撃力分、兵力を減らす
				playerData.soldierCount -= enemyData.attack;
			}
		}

		// ------兵数が０になったら戦場から除外------
		// 配列の先頭を指す付箋（イテレータ）を作成
		auto it = m_allEnemies.begin();
		while (it != m_allEnemies.end())
		{
			// チェック中の敵
			Enemy* e = *it;
			UnitData& enemyData = e->GetUnitData();

			// 兵数が尽きたとき
			if (enemyData.soldierCount <= 0)
			{
				// メモリの開放
				delete e;

				// 敵の数を減らす
				enemyAmount--; 

				// 配列から削除し、イテレータを進める
				it = m_allEnemies.erase(it);
			}
			else
			{
				// まだ生き残っていれば、手動でイテレータを次へ進める
				it++;
			}

		}

		// 敵の総数をカウント
		allEnemySoldier = 0; // 敵の総兵数をリセット
		for (Enemy* e : m_allEnemies)
		{
			allEnemySoldier += e->GetUnitData().soldierCount;
		}



	}

}

void SceneMain::Draw()
{
	// 背景の描画（画面サイズに合わせて補正）
	DrawGraph(0, -200, m_handle, TRUE);

	// プレイヤーの描画
	m_pPlayer->Draw();

	// 生き残っている全ての敵の描画
	for (Enemy* e : m_allEnemies)
	{
		e->Draw();
	}

	if (enemyAmount <= 0)
	{
		DrawString(900, 500, "You Win!", GetColor(0, 255, 0));
	}

	if (m_pPlayer->GetUnitData().soldierCount <= 0)
	{
		DrawString(900, 500, "Game Over", GetColor(255, 0, 0));
	}

	DrawFormatString(560, 20,GetColor(255, 255, 255), "敵総兵数 : %d人", allEnemySoldier);
}
