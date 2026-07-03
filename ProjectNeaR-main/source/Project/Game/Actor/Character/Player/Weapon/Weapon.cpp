#include "Weapon.h"
#include "../../../../../General/Model.h"
#include "../../../../../General/Collision/Rigidbody.h"
#include "../../../../../General/Collision/SphereCollider.h"

namespace
{
    //非戦闘時のオフセット値
    constexpr float kIdleOffsetY = 30.0f;

    //武器の大きさ調整
    constexpr float kRotaModelScaleOffset = 130.0f;

    //根本
    constexpr int kWeaponBot = 4;
    //剣先
    constexpr int kWeaponTop = 6;
}

Weapon::Weapon(std::shared_ptr<ActorData> actorData, std::weak_ptr<ActorManager> pActorManager) :
    Actor(actorData, Shape::Sphere, pActorManager),
    m_ownerHandle(-1),
    m_battleSlotIndex(0),
    m_idleSlotIndex(0),
    m_isBattle(false),
    m_up(Vector3::Up()),
    m_rotaCountFrame(0.0f),
    m_rotaFrame(0.0f),
    m_rotaSpeed(0.0f),
    m_rotaAngle(0.0f),
    m_isDraw(true)
{
    m_model->SetRotSpeed(1.0f);
}

Weapon::~Weapon() = default;

void Weapon::Init()
{
    // Physics に登録
    Collidable::Init();
}

void Weapon::Update()
{
    // 必要に応じて実装
}

void Weapon::Draw() const
{
    if (m_isDraw)
    {
        m_model->Draw();
    }
}

void Weapon::DrawShadow() const
{
    Draw();
}

void Weapon::End()
{
    // モデルを解放
    m_model->End();
    // Physics から解除
    Collidable::End();
}

void Weapon::OnCollide(const std::shared_ptr<Collidable> other)
{
    // 衝突時の処理（必要に応じて実装）
}

void Weapon::Complete()
{
    //現在のスロット
    int slotIndex = m_idleSlotIndex;
    if (m_isBattle)
    {
        slotIndex = m_battleSlotIndex;
    }

    //モデルフレームのローカルワールド行列を取得
    Matrix4x4 mat = Matrix4x4::ToMatrix4x4(MV1GetFrameLocalWorldMatrix(m_ownerHandle, slotIndex));

    //スケールを適用
    mat = mat * Matrix4x4::ScaleMatrix4x4(m_actorData->GetScale());

    //武器の位置を取得
    Vector3 weaponPos = MV1GetFramePosition(m_ownerHandle, slotIndex);

    //非戦闘時のみ Y軸にオフセットを加える
    if (!m_isBattle)
    {
        weaponPos.y += kIdleOffsetY;
    }
    else
    {
        //武器の回転
        UpdateThrowRota(weaponPos, mat);
    }

    //行列の平行移動部分を更新
    mat.mat[3][0] = weaponPos.x;
    mat.mat[3][1] = weaponPos.y;
    mat.mat[3][2] = weaponPos.z;

    //モデルにマトリクスをセット
    SetWeaponMat(mat);

    //モデル位置を更新
    m_model->SetPos(weaponPos.ToDxLibVector());

    //Rigidbody の位置を更新
    m_rb->m_pos = weaponPos;
    
    UpdateWeaponDir();
}

void Weapon::SetWeaponMat(Matrix4x4 mat)
{
    m_model->SetMatrix(mat);
}

Vector3 Weapon::GetStartPos() const
{
    //現在のスロット
    int slotIndex = m_idleSlotIndex;
    if (m_isBattle)
    {
        slotIndex = m_battleSlotIndex;
    }
    //武器の位置を取得
    Vector3 weaponPos = MV1GetFramePosition(m_ownerHandle, slotIndex);
    //モデル位置を更新
    m_model->SetPos(weaponPos.ToDxLibVector());
    return GetPos();
}

Vector3 Weapon::GetEndPos(float length) const
{
    return (m_up * length) + GetStartPos();
}

void Weapon::ThrowAndRoll(float length, Vector3 dir, Vector3 startPos, float frame, float rotaSpeed)
{
    //回転にかかるフレーム
    m_rotaFrame = frame;
    m_rotaCountFrame = 0.0f;

    //方向
    Vector3 vec = dir;
    if (vec.SqMagnitude() > 0.0f)
    {
        vec = vec.Normalize();
    }

    //移動先
    Vector3 targetPos = (vec * length) + startPos;
    m_targetPos = targetPos;

    //回転速度
    m_rotaSpeed = rotaSpeed;

    //角度リセット
    m_rotaAngle = 0.0f;
}

void Weapon::FinisiThrowAndRoll()
{
    m_rotaCountFrame = m_rotaFrame;
}

void Weapon::UpdateWeaponDir()
{
    //向きの更新
    int myHandle = m_model->GetModelHandle();
    m_up = Vector3(MV1GetFramePosition(myHandle, kWeaponTop)) - Vector3(MV1GetFramePosition(myHandle, kWeaponBot));
    if (m_up.SqMagnitude() > 0.0f)
    {
        m_up = m_up.Normalize();
    }
}


void Weapon::UpdateThrowRota(Vector3& weaponPos, Matrix4x4& mat)
{
    //回転するなら
    if (m_rotaCountFrame < m_rotaFrame)
    {
        float timeScale = GetTimeScale();

        m_rotaCountFrame += timeScale;

        //割合
        float rate = MathSub::ClampFloat((m_rotaCountFrame / m_rotaFrame) * 2.0f, 0.0f, 1.0f);

        //角度更新
        m_rotaAngle += m_rotaSpeed * timeScale;

        //移動
        Vector3 nextPos = Vector3::Lerp(GetPos(), m_targetPos, rate);
        weaponPos = nextPos;

        //回転
        mat = Matrix4x4::IdentityMat4x4();
        mat = mat * Matrix4x4::RotateZPositionMatrix4x4(nextPos, 90.0f * MyMath::DEG_2_RAD);
        mat = mat * Matrix4x4::RotateYPositionMatrix4x4(nextPos, m_rotaAngle * MyMath::DEG_2_RAD);

        //スケールを適用
        mat = mat * Matrix4x4::ScaleMatrix4x4(m_actorData->GetScale() * kRotaModelScaleOffset);
    }
}