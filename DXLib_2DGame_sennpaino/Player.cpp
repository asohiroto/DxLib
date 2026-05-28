#include"DxLib.h"
#include"Player.h"
#include"Character.h"
#include"Mouse.h"
#include"Pad.h"
#include<string>

Player::Player()
{
	m_handleFlag = LoadGraph("media/fc44.png");
}

Player::~Player()
{
	DeleteGraph(units[0].handle);
}

void Player::Init()
{
	m_pos = Vec2(900.0f, 500.0f); // 初期位置
	m_targetPos = m_pos;		  // 最初は目的地を初期位置に
	m_x = 0;					  // マウスの座標
	m_y = 0;					  //
	m_angle = 0.0f;				  // 向いている角度
	m_isMoving = false;			  // 最初は停止状態
	flagFlag = false;			  // 旗を表示しているか

	units.clear();				  // 配列の初期化

	UnitData king;
	king.name = std::string("王将");
	king.soldierCount = 1000;
	king.attack = 50;
	king.speed = 10.0f;
	king.handle = LoadGraph("media/ousyou.png");
	king.isEnemy = false;
	units.push_back(king);


}

void Player::Update()
{
	// クリック時に目的地を更新
	if (Mouse::IsTriggerLeft())
	{
		// マウスの座標を保存
		GetMousePoint(&m_x, &m_y);

		flagFlag = true;

		// 目的地を、保存したマウスの座標に更新
		m_targetPos = Vec2(static_cast<float>(m_x), static_cast<float>(m_y));
		m_isMoving = true; // 移動開始フラグ
	}

	// 移動中処理
	if (m_isMoving)
	{
		// 経路ベクトルを作成
		Vec2 toTarget;
		toTarget.x = m_targetPos.x - m_pos.x - 42.5f;
		toTarget.y = m_targetPos.y - m_pos.y - 56.5f;

		m_angle = atan2f(toTarget.y, toTarget.x);

		// 目的地までの距離を計算
		float distance = toTarget.length();

		// 目的地までの距離が速度よりも小さければ、到着したものとしてみなす
		if (distance <= units[0].speed)
		{
			m_pos.x = m_targetPos.x - 42.5f; // 目的地X座標に着地
			m_pos.y = m_targetPos.y - 56.5f; // 目的地Y座標に着地
			m_isMoving = false;	 // 移動終了
			flagFlag = false;
		}
		else
		{
			// 経路ベクトルの単位ベクトルを取得し、方向とする
			Vec2 direction = toTarget.getNormalize();
			m_pos += direction * units[0].speed;
		}
	}
}

void Player::Draw()
{
	// クリックした位置に旗を表示
	if (flagFlag)
	{
		DrawGraph(m_x - 32, m_y - 32, m_handleFlag, TRUE);
	}


	Character::Draw(m_pos.x, m_pos.y, m_angle, units[0].handle);
}
