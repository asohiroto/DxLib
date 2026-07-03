#pragma once
#include "PlayerStateAttackBase.h"
#include <memory>
#include <string>
class Actor;
class Player;
class AttackData;
class SwordAttack;
class PlayerStateHeavyAttack :
    public PlayerStateAttackBase, public std::enable_shared_from_this<PlayerStateHeavyAttack>
{
public:
    PlayerStateHeavyAttack(std::weak_ptr<Actor>  player, bool isWait,bool isDash = false, bool isJust = false);
    ~PlayerStateHeavyAttack();
    void Init()override;
    void Update() override;
private:
    //次の攻撃
    std::wstring m_nextAttackName;
    //状態遷移
    using UpdateFunc_t = void(PlayerStateHeavyAttack::*)(std::shared_ptr<Player> owner,Input& input);
    UpdateFunc_t m_update;
    //ジャスト回避
	bool m_isJust;
private:
    //チャージ攻撃をするか(チャージ中はtrueを返す)
    bool LoadNextChargeOrCombo(std::shared_ptr<Player> owner, Input& input, std::shared_ptr<Model> model);
    //移動か待機か
    void ChangeToMoveOrIdle(std::shared_ptr<Player> owner, Input& input);
	//攻撃開始処理
    void UpdateStartFrame(std::shared_ptr<Player> owner, std::shared_ptr<Weapon> weapon);

    //地上
    void GroundUpdate(std::shared_ptr<Player> owner, Input& input);
    //空中
    void AirUpdate(std::shared_ptr<Player> owner, Input& input);
};


