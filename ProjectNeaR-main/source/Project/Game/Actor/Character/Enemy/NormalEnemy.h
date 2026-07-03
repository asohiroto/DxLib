#pragma once
#include "EnemyBase.h"
#include "../../../../General/Math/MyMath.h"
#include "../../../../General/CSV/AnimData.h"
#include "../../../../General/CSV/AttackData.h"
#include <memory>
#include <map>
#include <string>

class ActorManager;
class CharacterStateBase;
class CharaStatusData;
class Input;
class ActorData;
class PlayerCamera;
class Weapon;
class CSVDataLoader;
class AttackBase;
class NormalEffect;
class NormalEnemy :
	public EnemyBase
{
public:
	NormalEnemy(std::shared_ptr<ActorData> actorData, std::shared_ptr<CharaStatusData> charaStatusData, std::weak_ptr<ActorManager> pActorManager);
	virtual ~NormalEnemy();
	//初期化処理
	void Init()override;
	//更新処理による結果の確定
	void Complete() override;
	//終了処理
	void End()override;

};

