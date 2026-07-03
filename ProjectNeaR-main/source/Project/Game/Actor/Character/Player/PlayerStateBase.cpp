#include "PlayerStateBase.h"
#include "Player.h"
#include "PlayerStateIdle.h"
#include "../../../../General/Input.h"

PlayerStateBase::PlayerStateBase(std::weak_ptr<Actor>  player, bool isWait) :
	CharacterStateBase(player, isWait)
{
}

PlayerStateBase::~PlayerStateBase()
{
}

Vector3 PlayerStateBase::InputMoveVec(std::shared_ptr<Player> owner, Input& input)
{
	auto rb = owner->GetRb();
	//ˆÚ“®
	Vector3 vec = Vector3::Zero();
	vec.x = static_cast<float>(input.GetStickInfo().leftStickX);
	vec.z = -static_cast<float>(input.GetStickInfo().leftStickY);
	if (vec.SqMagnitude() > 0.0f)
	{
		vec = vec.Normalize();
	}
	//ƒJƒƒ‰‚ÌŒü‚«‚É‡‚í‚¹‚ÄˆÚ“®•ûŒü‚ð•Ï‚¦‚é
	vec = owner->GetCameraRot() * vec;
	return vec;
}