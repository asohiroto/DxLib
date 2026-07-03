#pragma once
#include "../EnemyBase.h"
#include <memory>
class NormalEffect;
class Boss4 :
	public EnemyBase
{
public:
	Boss4(std::shared_ptr<ActorData> actorData, std::shared_ptr<CharaStatusData> charaStatusData, std::weak_ptr<ActorManager> pActorManager);
	virtual ~Boss4();
	//初期化処理
	void Init()override;
	//更新処理による結果の確定
	void Complete() override;
	//終了処理
	void End()override;

	//第二形態かどうかを設定
	bool IsSecondPhase() const { return m_isSecondPhase; };

	//第二形態へ移行
	void ChangeSecondPhase();

private:
	//オーラ
	std::weak_ptr<NormalEffect> m_auraEffect;
	//第二形態かどうか
	bool m_isSecondPhase;
	//第二形態のデータパス
	std::wstring m_secondPhaseDataPath;
};

