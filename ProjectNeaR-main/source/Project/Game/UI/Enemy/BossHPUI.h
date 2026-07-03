#pragma once
#include "../HPUIBase.h"
#include "../../../General/Math/MyMath.h"
#include <memory>
class CharaStatus;
class EnemyBase;
class BossHPUI :
	public HPUIBase
{
public:
	BossHPUI(std::shared_ptr<CharaStatus> charaStatus, std::weak_ptr<EnemyBase> pEnemy);
	~BossHPUI();
	//更新
	void Update() override;
	//描画
	void Draw()const override;
private:
	//持ち主
	std::weak_ptr<EnemyBase> m_pEnemy;
	//ボス
	int m_bossNameHandle;
};

