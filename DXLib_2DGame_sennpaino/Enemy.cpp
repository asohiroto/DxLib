#include "DxLib.h"
#include "Enemy.h"
#include"Character.h"
#include"Player.h"

Enemy::Enemy()
{
}

Enemy::~Enemy()
{
	// 読み込んだ画像のグラフィックハンドルを削除
	DeleteGraph(units[0].handle);
}

void Enemy::Init(Player* pPlayer, int type)
{

	// 座標のリセット
	m_pos = Vec2(0, 0);

	// m_pPlayerの初期化（最初はだれも狙っていない状態にする）
	m_pPlayer = nullptr;

	attackHandle = LoadGraph("media/attackArea.png");

	m_angle = 0;						// 向いている角度
	attackPos = Vec2(-100.0f, -100.0f); // 攻撃範囲の中心座標
	attackDir = Vec2();					// 攻撃の向き
	attackDirVer = Vec2();				// 攻撃の向きと垂直に交わるベクトル
	alpha = 128;						// 攻撃エリアの半透明度

	// 追跡対象となるプレイヤーのポインタを記憶
	m_pPlayer = pPlayer;

	// 配列の初期化
	units.clear();

	// 生成する部隊のデータを一時保存する箱
	UnitData data;

	// ------ タイプ０：桂馬のデータ ------
	if (type == 0)
	{
		data.name = "桂馬";
		data.soldierCount = 500;
		data.attack = 1;
		data.speed = 2.2f;
		data.attackDistance = 96;
		data.attackRange = 0.15f;
		data.handle = LoadGraph("media/keima.png");
		data.isEnemy = true;
	}

	// ------ タイプ１：金将のデータ ------
	if (type == 1)
	{
		data.name = "金将";
		data.soldierCount = 750;
		data.attack = 3;
		data.speed = 1.8f;
		data.attackDistance = 84;
		data.attackRange = 0.12f;
		data.handle = LoadGraph("media/kin.png");
		data.isEnemy = true;
	}

	// ------ タイプ２：銀将のデータ ------
	if (type == 2)
	{
		data.name = "銀将";
		data.soldierCount = 600;
		data.attack = 2;
		data.speed = 2.0f;
		data.attackDistance = 90;
		data.attackRange = 0.14f;
		data.handle = LoadGraph("media/gin.png");
		data.isEnemy = true;
	}

	// 決定した舞台データを配列に登録
	units.push_back(data);
}

void Enemy::Update()
{
	// プレイヤーの位置を目的地としベクトルを作成
	m_targetPos = m_pPlayer->m_pos;
	Vec2 toTarget = Vec2(m_targetPos.x - m_pos.x, m_targetPos.y - m_pos.y);

	// プレイヤーのいる角度
	m_angle = atan2f(toTarget.y, toTarget.x);

	// 距離を計算
	float distance = toTarget.length();

	// 目的地（プレイヤー）まで自身の速度で進む
	Vec2 direction = toTarget.getNormalize();
	m_pos += direction * units[0].speed;

	// 正面ベクトル
	attackDir = toTarget.getNormalize();

	// 正面ベクトルと垂直に交わるベクトルを求める
	attackDirVer.x = -attackDir.y;
	attackDirVer.y = attackDir.x;

	// 攻撃範囲の中心座標を求める
	attackPos = m_pos + attackDir * units[0].attackDistance + attackDirVer;
}

void Enemy::Draw()
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

	DrawRotaGraph(static_cast<int>(attackPos.x), static_cast<int>(attackPos.y), units[0].attackRange, m_angle, attackHandle, TRUE, FALSE);

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// 敵部隊の描画
	Character::Draw(m_pos.x, m_pos.y, m_angle, units[0].handle);
	
	// 頭上にデータの表示
	DrawFormatString(m_pos.x - 40, m_pos.y - 60, GetColor(255, 0, 0), " % s : % d人", units[0].name.c_str(), units[0].soldierCount);
}