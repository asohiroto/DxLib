#pragma once
#include "PlayerStateBase.h"
#include <memory>
class Actor;
class Player;
class Model;
class Input;
class Application;
class PlayerStateAvoid :
    public PlayerStateBase, public std::enable_shared_from_this<PlayerStateAvoid>
{
public:
    PlayerStateAvoid(std::weak_ptr<Actor>  player, bool isWait);
    ~PlayerStateAvoid();
    void Init()override;
    void Update() override;
private:
    //移動方向
    Vector3 m_avoidDir;
    //速度
    float m_speed;
    //エンド
    float m_endSpeed;
    //ジャスト回避成功
    bool m_isJustAvoid;
    //ジャスト回避が終了するフレーム
    float m_finishJustAvoid;
    //ジャスト回避終了後に行う行動
    bool m_isLightAttack;
    bool m_isHeavyAttack;
    //後退してるか
	bool m_isBack;
private:
    //ジャスト回避初期化
    void InitJustAvoid(std::shared_ptr<Model> model, std::shared_ptr<Player> owner);
    //ジャスト回避更新
    void UpdateJustAvoid(std::shared_ptr<Player> owner, std::shared_ptr<Model> model, Application& app);
    //回避移動
    void MoveAvoid(Input& input, Application& app, std::shared_ptr<Player> owner);
};
