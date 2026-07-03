#pragma once
#include "PlayerStateAttackBase.h"
#include <memory>
class Actor;
class AttackData;
class SwordAttack;
class Model;
class Player;
class PlayerStateLightAttack :
    public PlayerStateAttackBase, public std::enable_shared_from_this<PlayerStateLightAttack>
{
public:
    PlayerStateLightAttack(std::weak_ptr<Actor> player, bool isWait, bool isJump = false, bool isJust = false);
    ~PlayerStateLightAttack();
    void Init()override;
    void Update() override;
private:
    //次の攻撃
    void NextAttack(bool isChargeAttack, std::shared_ptr<Player> owner, std::shared_ptr<Model> model);
    //攻撃開始処理
    void UpdateStartAttack(std::shared_ptr<Player>& owner, std::shared_ptr<Weapon>& weapon);
    //移動か待機か
    void ChangeToMoveOrIdle(std::shared_ptr<Player> owner, Input& input);
private:
    //ジャスト回避
    bool m_isJust;
};


