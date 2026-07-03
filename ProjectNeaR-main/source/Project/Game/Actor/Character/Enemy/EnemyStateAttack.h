#pragma once
#include "EnemyStateBase.h"
#include <memory>
class Actor;
class AttackData;
class AttackBase;
class EnemyBase;
class EnemyStateAttack :
	public EnemyStateBase, public std::enable_shared_from_this<EnemyStateAttack>
{
public:
	EnemyStateAttack(std::weak_ptr<Actor> enemy, bool isWait,std::shared_ptr<AttackData> attackData);
	virtual ~EnemyStateAttack();
	virtual void Init() override;
	virtual void Update() override;
protected:
	//ƒtƒŒ[ƒ€‚É‚æ‚éUŒ‚‚ÌXV
	virtual void UpdateAttackFrame(std::shared_ptr<EnemyBase> owner);
	//UŒ‚íœ
	void DeleteAttack();
	//UŒ‚ì¬
	virtual void CreateAttack(std::shared_ptr<EnemyBase> owner);
	//‘½’iƒqƒbƒgUŒ‚“Ç‚İ‚İ
	virtual void ComboAttack(std::shared_ptr<EnemyBase> owner);
	//UŒ‚ˆÚ“®XV
	virtual void UpdateMove(std::shared_ptr<EnemyBase> owner, std::shared_ptr<Model> model);
	//UŒ‚ˆÊ’u‚ÌXV
	virtual void UpdateAttackPos(std::shared_ptr<EnemyBase> owner);
protected:
	//UŒ‚ƒf[ƒ^
	std::shared_ptr<AttackData> m_attackData;
	//UŒ‚”­¶
	bool m_isAppearedAttack;
	//UŒ‚‚ÌQÆ
	std::weak_ptr<AttackBase> m_pAttack;
	//’e‚ğ‘Å‚Á‚½
	bool m_isShotBullet;

};

