#pragma once
#include "../EnemyBase.h"
class Boss2Arm :
    public EnemyBase
{
public:
	Boss2Arm(std::shared_ptr<ActorData> actorData, std::shared_ptr<CharaStatusData> charaStatusData, std::weak_ptr<ActorManager> pActorManager);
	virtual ~Boss2Arm();
	//初期化処理
	void Init()override;
	//更新処理
	void Update() override;
	//更新処理による結果の確定
	void Complete() override;
	//終了処理
	void End()override;

	//フレームインデックスのセット
	void SetArmIndex(int startIndex, int endIndex);
	void SetArmModel(int modelHandle);
private:
	//腕を構成するフレームインデックス
	int m_startFrameIndex;
	int m_endFrameIndex;
	int m_modelHandle;
private:
	//腕の位置更新
	void UpdateArmPos();
};

