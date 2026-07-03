#pragma once
#include "../CharacterStateBase.h"
#include <memory>
#include "../../../../General/Math/MyMath.h"
#include <string>
class Actor;
class Player;
class ActorManager;
class Input;
class PlayerStateBase abstract:
	public CharacterStateBase
{
public:
	PlayerStateBase(std::weak_ptr<Actor> player, bool isWait);
	virtual ~PlayerStateBase();
protected:
	//ˆÚ“®•ûŒü
	Vector3 InputMoveVec(std::shared_ptr<Player> owner,Input& input);

};

