#include "MagicCollision.h"
#include "Players/Player.h"
#include "Enemys/Enemy.h"
#include "Bases/MagicBase.h"
#include <DxLib.h>
#include <EffekseerForDXLib.h>

namespace
{
	// エフェクト再生高さの補正値
	constexpr float EFFECT_HEIGHT_OFFSET = 300.0f;
	// プレイヤーの使うビームのダメージ
	constexpr int BEAM_DAMAGE = 300;
}

MagicCollision::MagicCollision() :
	_isPlayerHit(false),
	_wasPlayerHit(false),
	_isEnemyHit(false),
	_wasEnemyHit(false),
	_hitEnemyMagicInd(-1),
	_hitPlayerMagicInd(-1),
	_hitEffectH(-1),
	_isJustDodge(false),
	_wasJustDodge(false)
{
}

MagicCollision::~MagicCollision()
{
}

void MagicCollision::Init(int handle)
{
	_hitEffectH = handle;
}

void MagicCollision::End()
{
}

void MagicCollision::Update(std::shared_ptr<Player> pPlayer, std::shared_ptr<Enemy>pEnemy, MagicList& playerList, MagicList& enemyList)
{
	Character::CharacterData player = pPlayer->GetPlayerData();
	Character::CharacterData enemy = pEnemy->GetEnemyData();

	_wasJustDodge = _isJustDodge;
	_wasPlayerHit = _isPlayerHit;
	_wasEnemyHit = _isEnemyHit;

	_isJustDodge = false;

	_hitEnemyMagicInd = PlayerHitCheck(player, enemyList);
	_hitPlayerMagicInd = EnemyHitCheck(enemy, playerList);

	if (_hitEnemyMagicInd >= 0 && !pPlayer->IsDodge()) _isPlayerHit = true;
	else _isPlayerHit = false;

	if (_hitPlayerMagicInd >= 0) _isEnemyHit = true;
	else _isEnemyHit = false;

	if (IsPlayerHit())
	{
		const auto& magic = enemyList[_hitEnemyMagicInd];

		pPlayer->SetHit(magic.damage);

		VECTOR effectPos = pPlayer->GetPos();

		int handle = PlayEffekseer3DEffect(_hitEffectH);
		SetPosPlayingEffekseer3DEffect(handle, effectPos.x, effectPos.y + EFFECT_HEIGHT_OFFSET, effectPos.z);

		if (magic.type == MagicBase::MagicType::MagicFury || magic.type == MagicBase::MagicType::MagicBeam)
		{

		}
		else
			enemyList[_hitEnemyMagicInd].isExist = false;
	}

	if (IsEnemyHit())
	{
		const auto& magic = playerList[_hitPlayerMagicInd];

		if (magic.type == MagicBase::MagicType::MagicBeam)
			pEnemy->SetHit(BEAM_DAMAGE);
		else
			pEnemy->SetHit(magic.damage);

		pPlayer->SetUltCharge(magic.chargeAmount);

		VECTOR effectPos = pEnemy->GetPos();

		int handle = PlayEffekseer3DEffect(_hitEffectH);
		SetPosPlayingEffekseer3DEffect(handle, effectPos.x, effectPos.y + EFFECT_HEIGHT_OFFSET, effectPos.z);

		if (magic.type == MagicBase::MagicType::MagicFury || magic.type == MagicBase::MagicType::MagicBeam)
		{

		}
		else
			playerList[_hitPlayerMagicInd].isExist = false;
	}

	if (_isJustDodge && !_wasJustDodge)
		pPlayer->JustDodgeEffect();
}

void MagicCollision::Draw()
{
}

int MagicCollision::PlayerHitCheck(const Character::CharacterData& player, const MagicList& enemyList)
{
	// 敵が撃った魔法全てで順に計算
	for (int i = 0; i < enemyList.size(); i++)
	{
		if (!enemyList[i].isExist) continue;

		float distance = 0.0f;

		if (enemyList[i].type == MagicBase::MagicType::MagicBeam || enemyList[i].type == MagicBase::MagicType::MagicBeam)
		{
			distance = Segment_Segment_MinLength
			(player.segmentStPos, player.segmentEndPos,
				enemyList[i].segmentStPos, enemyList[i].segmentEndPos);
		}
		else
		{
			// 距離を計測
			distance = Segment_Point_MinLength(player.segmentStPos, player.segmentEndPos, enemyList[i].pos);
		}

		// めり込みの深さを計算
		float dipth = player.radius + enemyList[i].radius - distance;

		// ジャスト回避判定とのめり込みの深さを計算
		if (player.justRadius > 0.0f)
		{
			float justDipth = player.justRadius + enemyList[i].radius - distance;

			if (justDipth >= 0)
				_isJustDodge = true;
		}

		// 当たっている番号を返す
		if (dipth >= 0)
		{
			return i;
		}
	}
	return -1;
}

int MagicCollision::EnemyHitCheck(const Character::CharacterData& enemy, const MagicList& playerList)
{

	for (int i = 0; i < playerList.size(); i++)
	{
		if (!playerList[i].isExist) continue;

		float distance = 0.0f;

		if (playerList[i].type == MagicBase::MagicType::MagicFury || playerList[i].type == MagicBase::MagicType::MagicBeam)
		{
			distance = Segment_Segment_MinLength
			(enemy.segmentStPos, enemy.segmentEndPos,
				playerList[i].segmentStPos, playerList[i].segmentEndPos);
		}
		else
		{
			distance = Segment_Point_MinLength(enemy.segmentStPos, enemy.segmentEndPos, playerList[i].pos);
		}

		float dipth = enemy.radius + playerList[i].radius - distance;

		if (dipth >= 0)
		{
			return i;
		}
	}
	return -1;
}
