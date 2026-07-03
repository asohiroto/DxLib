#pragma once
#include "../EnemyBase.h"
#include <memory>
class Boss2Arm;
class CharaStatusData;
class Boss2 :
	public EnemyBase
{
public:
	Boss2(std::shared_ptr<ActorData> actorData, std::shared_ptr<CharaStatusData> charaStatusData, std::weak_ptr<ActorManager> pActorManager);
	virtual ~Boss2();
	//初期化処理
	void Init()override;
	//更新処理
	void Update() override;
	//更新処理による結果の確定
	void Complete() override;
	//終了処理
	void End()override;

	//腕が破壊されたか
	bool IsDestroyedRightArm() { return m_rightArm.expired(); };
	bool IsDestroyedLeftArm() { return m_leftArm.expired(); };

	//腕のトリガー化
	void EnableTriggerArm();
	void DisableTriggerArm();

	enum class ArmIndex : int
	{
		RightArm = -1,
		LeftArm = -2,
	};

private:
	//アーム初期化
	void InitArm(CSVDataLoader& csvLoader, std::wstring pathData);
	//腕の状態に合わせたモデルを初期化
	void InitArmStateModel(CSVDataLoader& csvLoader, std::wstring pathData);
	//腕の状態
	void CheckArmState();
private:
	//腕
	std::weak_ptr<Boss2Arm> m_leftArm;
	std::weak_ptr<Boss2Arm> m_rightArm;
	std::shared_ptr<CharaStatusData> m_charaStatusData;

	//腕の状態に合わせたモデル
	int m_noRightHandle;
	int m_noLeftHandle;
	int m_noBothHandle;
	bool m_isInitNoRight;
	bool m_isInitNoLeft;
	bool m_isInitNoBoth;
};

