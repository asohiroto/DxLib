#pragma once
#include "../../../Actor.h"
class ActorData;
class ActorManager;
class Weapon :
    public Actor
{
public:
    Weapon(std::shared_ptr<ActorData> actorData, std::weak_ptr<ActorManager> pActorManager);
    virtual ~Weapon();
    /// <summary>
    /// 初期化処理
    /// </summary>
    virtual void Init() override;
    /// <summary>
    /// 位置などの更新を行う(引数なし)
    /// </summary>
    virtual void Update() override;
    /// <summary>
    /// 対象のアクターを表示
    /// </summary>
    virtual void Draw() const override;
    virtual void DrawShadow() const override;
    /// <summary>
    /// 終了処理
    /// </summary>
    virtual void End() override;
    /// <summary>
    /// 衝突したときのイベント関数
    /// </summary>
    /// <param name="other"></param>
    virtual void OnCollide(const std::shared_ptr<Collidable> other)override;
    /// <summary>
    /// Updateの処理によって起こった衝突処理などの処理の確定
    /// </summary>
    virtual void Complete() override;

    //武器の行列
    void SetWeaponMat(Matrix4x4 mat);
    //フレーム番号
    void SetFrameIndex(int battleSlotIndex, int idleSlotIndex)
    {
        m_battleSlotIndex = battleSlotIndex;
        m_idleSlotIndex = idleSlotIndex;
    };
    //持ち主のハンドル
    void SetOwnerHandle(int ownerHandle) { m_ownerHandle = ownerHandle; };
    
    //戦闘中か
    bool IsBattle()const { return m_isBattle; };
    void SetIsBattle(bool isBattle) { m_isBattle = isBattle; };

    //剣先
    Vector3 GetUP()const { return m_up; };

    //始点
    Vector3 GetStartPos()const;
    //終点
    Vector3 GetEndPos(float length)const;

    //くるくる回る
    void ThrowAndRoll(float length,Vector3 dir, Vector3 startPos ,float frame,float rotaSpeed);
    void FinisiThrowAndRoll();

    //描画
    void SetIsDraw(bool isDraw) { m_isDraw = isDraw; };
private:
    //持ち主のハンドル
    int m_ownerHandle;

    //戦闘中のフレーム番号
    int m_battleSlotIndex;

    //非戦闘のフレーム番号
    int m_idleSlotIndex;

    //戦闘中かどうか
    bool m_isBattle;

    //剣先
    Vector3 m_up;

    //回転フレーム
    float m_rotaFrame;
    float m_rotaCountFrame;

    //回転速度
    float m_rotaSpeed;
    float m_rotaAngle;

    //移動先
    Vector3 m_targetPos;

    //描画
    bool m_isDraw;
private:
    //向きの更新
    void UpdateWeaponDir();
    //回転
    void UpdateThrowRota(Vector3& weaponPos, Matrix4x4& mat);

};

