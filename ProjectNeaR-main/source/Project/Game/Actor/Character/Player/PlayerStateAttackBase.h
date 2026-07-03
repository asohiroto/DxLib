#pragma once
#include "PlayerStateBase.h"
#include <memory>
#include <string>
class Actor;
class Player;
class AttackData;
class AttackBase;
class Weapon;
class PlayerStateAttackBase abstract:
    public PlayerStateBase
{
public:
    PlayerStateAttackBase(std::weak_ptr<Actor>  player, bool isWait);
    virtual ~PlayerStateAttackBase();
protected:
    //攻撃データ
    std::shared_ptr<AttackData> m_attackData;
    //攻撃発生
    bool m_isAppearedAttack;
    //攻撃の参照
    std::weak_ptr<AttackBase> m_pAttack;
    //チャージフレーム
    float m_chargeCountFrame;
protected:
    //攻撃削除
    void DeleteAttack();
    //攻撃発生
    void CreateAttack(std::shared_ptr<Player> owner, std::shared_ptr<Weapon> weapon);
    //多段ヒット攻撃
    void LoadNextMultipleHitAttack(std::shared_ptr<Player> owner, std::shared_ptr<Weapon> weapon);
    //攻撃位置の更新
    void UpdateAttackPosition(std::shared_ptr<Player> owner, std::shared_ptr<Weapon> weapon);
    //攻撃による移動
    void UpdateMove(std::shared_ptr<Player> owner, Input& input, std::shared_ptr<Model> model);
};


