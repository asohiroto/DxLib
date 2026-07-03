#pragma once
#include "../../General/Collision/Collidable.h"
#include "../../General/CharaStatus.h"
#include <list>
#include <memory>
#include "../Actor/Actor.h"
#include <unordered_set>
#include "../../General/CSV/AttackData.h"

class Actor;
class AttackData;
class CharacterBase;
class NormalEffect;
class AttackBase abstract :
    public Collidable
{
public:
    AttackBase(Shape shape, std::shared_ptr<AttackData> attackData, std::weak_ptr<CharacterBase> pOwner);
    ~AttackBase();
    //初期化処理
    virtual void Init()override;
    //更新処理
    virtual void Update();
    //描画
    virtual void Draw()const abstract;
    //衝突イベント
    virtual void OnCollide(const std::shared_ptr<Collidable> other)override;
    //位置確定
    virtual void Complete()override {};
    //終了処理
    virtual void End()override;

    //削除
    bool IsDelete();
    void Delete();

    //IDリセット
    void ResetHitID();

    //当たったことがある
    bool IsHit()const { return m_isHit; };

    //ヒットストップをするか
    bool IsRequestHitStop()const { return m_isRequestHitStop; };
    //ヒットストップ
    int GetHitStopFrame()const { return m_hitStopFrame; };
    //揺れの強さ
    int GetHitStopShakePower()const { return m_hitStopShakePower; };

    //攻撃のタグ
    GameTag GetOwnerTag()const;

	//当たったIDか
    bool IsHitID(int otherID);
protected:
    //威力
    int m_attackPower;
    //攻撃の重さ
    CharaStatus::AttackWeight m_attackWeight;
    //持続フレーム
    float m_keepFrame;
    //ノックバック垂直
    float m_knockBackV;
    //ノックバック大きさ
    float m_knockBackPower;

    //ヒットストップフレーム
    int m_hitStopFrame;
    //揺れの強さ
    int m_hitStopShakePower;

    //当てたことのあるActorのIDを覚えておく
    //unordered_setのリンク
    //https://jp-seemore.com/iot/24421/#google_vignette
    std::unordered_set<int> m_hitId;

    //持ち主
    std::weak_ptr<CharacterBase> m_pOwner;


    //当たったことがある
    bool m_isHit;

    //ヒットストップをする
    bool m_isRequestHitStop;

    //ヒットエフェクトパス
    std::wstring m_hitEffectPath;

    //攻撃エフェクトパス
    std::wstring m_attackEffectPath;

    //攻撃エフェクト
    std::weak_ptr<NormalEffect> m_attackEffect;

    //基準位置情報
    AttackData::AttackOriginPosType m_originPosType;

    //中心座標
    Vector3 m_oriPos;

    //ヒットSE
    std::wstring m_hitSEPath;
};

