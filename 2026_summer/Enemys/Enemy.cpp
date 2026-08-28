#include "Enemy.h"
#include "EnemyManager.h"
#include "Anims/AnimManager.h"
#include <algorithm>

namespace
{
	// 敵の半径
	constexpr float RADIUS = 130.0f;
	// 線分の長さ
	constexpr float SEGMENT_LENGTH = 270.0f;
	// 線分を高く補正する
	constexpr float SEGMENT_HEIGHT_COR = 100.0f;
	// やられフレーム数
	constexpr int DAMAGED_FRAME = 10;
	// 【デバッグ用】当たったときの当たり判定の色
	constexpr int DAMAGED_COLOR = 0x00ff00;
	// 【デバッグ用】通常の当たり判定の色
	constexpr int NORM_COLOR = 0xff0000;
	// 最大体力
	constexpr int MAX_HP = 5000;
	// X軸方向の移動制限
	constexpr float POS_LIMIT_X = 3000.0f;
	// Z軸方向の移動制限
	constexpr float POS_LIMIT_Z = 3000.0f;
}

Enemy::Enemy() :
	_enemyUnit(),
	_damagedCount(0),
	p_AManager(nullptr)
{
}

Enemy::~Enemy()
{

}

void Enemy::Init(int handle)
{
	_enemyUnit.pos = VGet(0.0f, 0.0f, 0.0f);
	_enemyUnit.modelH = handle;
	_enemyUnit.radius = RADIUS;
	_enemyUnit.color = NORM_COLOR;
	_enemyUnit.isHit = false;
	_enemyUnit.nowState = CharacterState::MoveAway;

	// ステータスを決定
	_enemyUnit.maxHp = MAX_HP;
	_enemyUnit.hp = _enemyUnit.maxHp;

	p_AManager = std::make_shared<AnimManager>();
	p_AManager->Init(_enemyUnit.modelH);

	// モデルを拡大
	MV1SetScale(_enemyUnit.modelH, VGet(2.5f, 2.5f, 2.5f));
}

void Enemy::End()
{

}

void Enemy::Update(float angle, std::shared_ptr<EnemyManager> pEManager)
{
	// 【当たり判定用】線分の始点と終点を設定
	_enemyUnit.segmentStPos = VGet(_enemyUnit.pos.x, SEGMENT_HEIGHT_COR, _enemyUnit.pos.z);
	_enemyUnit.segmentEndPos = VGet(_enemyUnit.pos.x, SEGMENT_HEIGHT_COR + SEGMENT_LENGTH, _enemyUnit.pos.z);

	_damagedCount++;

	// 埋まり防止用
	if (_enemyUnit.pos.y <= 0.0f) _enemyUnit.pos.y = 0.0f;

	// アニメの切り替え
	if (pEManager->CanAnimChange())
		p_AManager->AnimChange(TranslateState(_enemyUnit.nowState), false, 60);

	_enemyUnit.pos.x = std::clamp(_enemyUnit.pos.x, -POS_LIMIT_X, POS_LIMIT_X);
	_enemyUnit.pos.z = std::clamp(_enemyUnit.pos.z, -POS_LIMIT_Z, POS_LIMIT_Z);

	if (_enemyUnit.isHit)
	{
		if (_damagedCount <= DAMAGED_FRAME)
		{
			_enemyUnit.color = DAMAGED_COLOR;
		}
		else if (_damagedCount > DAMAGED_FRAME)
		{
			_enemyUnit.color = NORM_COLOR;
			_enemyUnit.isHit = false;
		}
	}

	MV1SetRotationXYZ(_enemyUnit.modelH, VGet(0.0f, angle, 0.0f));
	p_AManager->Update();
}

void Enemy::Draw()
{
	MV1SetPosition(_enemyUnit.modelH, _enemyUnit.pos);
	MV1DrawModel(_enemyUnit.modelH);

#ifdef _DEBUG
	DrawHitBox(_enemyUnit);

	DrawFormatString(0, 100, 0xffffff, "EneHp : %d / EneMaxHp : %d", _enemyUnit.hp, _enemyUnit.maxHp);
#endif

}

void Enemy::SetHit(int damage)
{
	if (!_enemyUnit.isHit)
	{
		_enemyUnit.isHit = true;
		_damagedCount = 0;

		_enemyUnit.hp -= damage;
	}
}

void Enemy::ChangeState(CharacterState state)
{
	_enemyUnit.nowState = state;
}
