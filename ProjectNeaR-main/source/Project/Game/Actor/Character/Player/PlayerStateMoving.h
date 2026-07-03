#pragma once
#include "PlayerStateBase.h"
class Actor;
class Player;
class Input;
class SE;
class PlayerStateMoving :
    public PlayerStateBase, public std::enable_shared_from_this<PlayerStateMoving>
{
public:
    PlayerStateMoving(std::weak_ptr<Actor>  player, bool isWait,bool isDash);
    ~PlayerStateMoving();
    void Init()override;
    void Update() override;
private:
    //速度
    float m_speed;
    //ダッシュ
    bool m_isDash;
private:
    //アニメーション切り替え
    void ChangeWalkAnim(std::shared_ptr<Player> owner, Input& input);
    //移動
    void Move(std::shared_ptr<Player> owner, Input& input);
    //SE
	std::weak_ptr<SE> m_moveSE;
};

