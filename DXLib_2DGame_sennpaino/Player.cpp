#include"DxLib.h"
#include"Player.h"
#include"Character.h"
#include"Mouse.h"
#include<string>


Player::Player()
{
}

Player::~Player()
{
	// 読み込んだ部隊のグラフィックハンドルを削除
	DeleteGraph(units[0].handle);
}

void Player::Init()
{

	// 移動先の目印となる旗の画像の読み込み
	m_handleFlag = LoadGraph("media/fc44.png");

	// 攻撃エリアの画像の読み込み
	attackHandle = LoadGraph("media/attackArea.png");

	m_pos = Vec2(900.0f, 500.0f);		// 初期位置
	m_x = 0;							// マウスの座標
	m_y = 0;							//
	m_angle = 0.0f;						// 向いている角度
	m_isMoving = false;					// 最初は停止状態
	flagFlag = false;					// 旗を表示しているか
	attackPos = Vec2(-100.0f, -100.0f); // 攻撃範囲の中心座標
	attackDir = Vec2();					// 攻撃の向き
	attackDirVer = Vec2();				// 攻撃の向きと垂直に交わるベクトル
	alpha = 128;						// 攻撃エリアの半透明度
	units.clear();						// 配列の初期化

	// ------プレイヤー部隊のデータ設定------
	UnitData king;
	king.name = "王将";
	king.target = "旗";
	king.soldierCount = 850;
	king.attack = 3;
	king.speed = 3.0f;
	king.attackDistance = 128;
	king.attackRange = 0.2f;
	king.handle = LoadGraph("media/ousyou.png");
	king.isEnemy = false;
	units.push_back(king);


	printfDx("%d", m_handleFlag);

}

void Player::Update()
{
	// クリック時に目的地を更新
	if (Mouse::IsTriggerLeft())
	{
		// マウスの座標を保存
		GetMousePoint(&m_x, &m_y);

		// 旗の表示フラグON
		flagFlag = true;

		// 目的地を、保存したマウスの座標に更新
		m_targetPos = Vec2(static_cast<float>(m_x), static_cast<float>(m_y));
		m_isMoving = true; // 移動開始フラグ
	}

	// 移動中処理
	if (m_isMoving)
	{
		// 経路ベクトルを作成（画像の中心点を考慮して引く）
		Vec2 toTarget;
		toTarget.x = m_targetPos.x - m_pos.x - 42.5f;
		toTarget.y = m_targetPos.y - m_pos.y - 56.5f;

		// 目的地までの角度を計算
		m_angle = atan2f(toTarget.y, toTarget.x);

		// 目的地までの距離を計算
		float distance = toTarget.length();

		// 目的地までの距離が速度よりも小さければ、到着したものとしてみなす
		if (distance <= units[0].speed)
		{
			m_pos.x = m_targetPos.x - 42.5f; // 目的地X座標に着地
			m_pos.y = m_targetPos.y - 56.5f; // 目的地Y座標に着地
			m_isMoving = false;				 // 移動終了
			flagFlag = false;				 // 移動終了したので旗を消す
		}
		else
		{
			// 経路ベクトルの単位ベクトルを取得し、方向とする
			Vec2 direction = toTarget.getNormalize();
			// 目的地まで自身の速度で進む
			m_pos += direction * units[0].speed;
		}
		// 正面ベクトル
		attackDir = toTarget.getNormalize();

		// 正面ベクトルと垂直に交わるベクトルを求める
		attackDirVer.x = -attackDir.y;
		attackDirVer.y = attackDir.x;

		// 攻撃範囲の中心座標を求める
		attackPos = m_pos + attackDir * units[0].attackDistance + attackDirVer;
	}
}

void Player::Draw()
{
	// クリックした位置に旗を表示
	if (flagFlag)
	{
		// 画像の中心をクリック位置にするために補正
		DrawGraph(m_x - 32, m_y - 32, m_handleFlag, TRUE);
	}

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);

	DrawRotaGraph(static_cast<int>(attackPos.x), static_cast<int>(attackPos.y), units[0].attackRange, m_angle, attackHandle, TRUE, FALSE);

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// キャラクター本体の描画
	Character::Draw(m_pos.x, m_pos.y, m_angle, units[0].handle);

	// 頭上にデータの表示
	DrawFormatString(m_pos.x - 40, m_pos.y - 60, GetColor(255, 255, 255), " % s : % d人", units[0].name.c_str(), units[0].soldierCount);

}
