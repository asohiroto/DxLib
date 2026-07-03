#pragma once
#include "EnemyStateBase.h"
class Actor;
class EnemyBase;
class EnemyStateHit :
	public EnemyStateBase, public std::enable_shared_from_this<EnemyStateHit>
{
public:
	EnemyStateHit(std::weak_ptr<Actor> enemy, bool isWait);
	~EnemyStateHit();
	void Init()override;
	void Update() override;
private:
	//最低上昇量を保証する
	void MinUpVecY(std::shared_ptr<EnemyBase> owner);
	//地上か空中かでアニメーション変更
	void ChangeNormalOrFall(std::shared_ptr<EnemyBase> owner);
private:
	bool m_isFall;
};

