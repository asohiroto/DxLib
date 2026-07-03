#include "Model.h"
#include "Math/MyMath.h"
#include "Animator.h"
#include "StringUtil.h"
#include <array>
#include <cassert>
#include <string>

namespace
{
	constexpr float kAnimSpeed = 0.5f;//再生速度
	//回転速度
	constexpr int kModelRotateSpeed = 10;
}

Model::Model(int modelHandle, VECTOR pos) :
	m_modelHandle(modelHandle),
	m_forward{ 0.0f,0.0f,1.0f },
	m_nextForward{ 0.0f,0.0f,1.0f },
	m_rotation(Quaternion::AngleAxis(180 * MyMath::DEG_2_RAD, Vector3::Up())),
	m_rotaQ(Quaternion::IdentityQ()),
	m_rotaFrame(0),
	m_rotaSpeed(kModelRotateSpeed),
	m_pos(pos),
	m_scale{ 1.0f,1.0f,1.0f },
	m_modelHeightAdjust(0.0f),
	m_timeScale(1.0f)
{
	//アニメーション
	m_animator = std::make_unique<Animator>();
	if (m_modelHandle == -1)return;
	//座標
	DxLib::MV1SetPosition(m_modelHandle, m_pos.ToDxLibVector());
}

Model::Model(int modelHandle, VECTOR pos, Vector3 forward) :
	m_modelHandle(modelHandle),
	m_forward(forward),
	m_nextForward(forward),
	m_rotation(Quaternion::IdentityQ()),
	m_rotaQ(Quaternion::IdentityQ()),
	m_rotaFrame(0),
	m_pos(pos),
	m_scale{ 1.0f,1.0f,1.0f },
	m_beforeSetDir{ forward.XZ() },
	m_modelHeightAdjust(0.0f),
	m_timeScale(1.0f),
	m_rotaSpeed(1.0f)
{
	//アニメーション
	m_animator = std::make_unique<Animator>();
	if (m_modelHandle == -1)return;
	//座標
	DxLib::MV1SetPosition(m_modelHandle, m_pos.ToDxLibVector());
}

Model::~Model()
{
}

void Model::Update()
{
	if (m_modelHandle == -1)return;

	//アニメーションの更新
	m_animator->PlayAnim(m_modelHandle);

	//向きの更新
	if (m_rotaFrame > 0 && m_timeScale > 0.0f)
	{
		--m_rotaFrame;
		//回転
		m_rotation = m_rotaQ * m_rotation;
		//正規化
		if (m_rotation.Magnitude() > 0.0f && m_rotaQ.w < 1.0f)
		{
			m_rotation = m_rotation.NormQ();
			DxLib::MV1SetRotationMatrix(m_modelHandle, m_rotation.GetMatrix().ToDxLibMATRIX());
		}
		//前ベクトル
		m_forward = m_rotaQ * m_forward;
		//正規化
		if (m_forward.Magnitude() > 0.0f)m_forward = m_forward.Normalize();
	}
}

void Model::Draw() const
{
	//非同期読み込み中なら早期リターン
	if (CheckHandleASyncLoad(m_modelHandle))return;
	if (m_modelHandle == -1)return;
	//描画
	auto err = DxLib::MV1DrawModel(m_modelHandle);
	assert(err != -1 && L"モデルが描画できません");
}

void Model::End()
{
	if (m_modelHandle == -1)return;
	DxLib::MV1DeleteModel(m_modelHandle);
}

void Model::SetPos(VECTOR pos)
{
	m_pos = pos;
}

void Model::SetScale(VECTOR scale)
{
	m_scale = scale;
}

void Model::SetRot(VECTOR rot)
{
	//オイラー角から回転
	m_rotaQ = Quaternion::Euler(rot.x, rot.y, rot.z);
	//回転
	m_rotation = m_rotaQ * m_rotation;
	//正規化
	if (m_rotation.Magnitude() > 0.0f && m_rotaQ.w < 1.0f)
	{
		m_rotation = m_rotation.NormQ();
		DxLib::MV1SetRotationMatrix(m_modelHandle, m_rotation.GetMatrix().ToDxLibMATRIX());
	}
	//前ベクトル
	m_forward = m_rotaQ * m_forward;
	//正規化
	if (m_forward.Magnitude() > 0.0f)m_forward = m_forward.Normalize();
}

void Model::SetDir(Vector2 vec)
{
	//向きが決められないのでリターン
	if (vec.Magnitude() <= 0.0f)	return;
	//向きを計算
	Vector2 dir = vec;
	dir = dir.Normalize();
	//前回と同じかつ向きも同じなら何もしない
	if (m_beforeSetDir == dir && m_forward.XZ() == dir)return; 
	m_beforeSetDir = dir; //前回の向きを記録
	//角度
	float angle = Vector2::GetRad(m_forward.XZ(), dir);
	//回転軸
	Vector3 axis = m_forward.Cross(dir.XZ());
	if (axis.SqMagnitude() == 0.0f)
	{
		axis = Vector3::Up();
	}
	//回転クォータニオン作成
	m_rotaQ = Quaternion::AngleAxis(angle/ m_rotaSpeed, axis);
	//フレームをセット
	m_rotaFrame = m_rotaSpeed;
	//次の正面ベクトルを記録
	m_nextForward = dir.XZ();
}

void Model::SetDir(Vector3 vec)
{
	if (vec.SqMagnitude() <= 0.0f) return;

	Vector3 dir = vec.Normalize();

	// ほぼ同じなら無視
	if ((m_forward - dir).SqMagnitude() < 1e-6f) return;

	// 角度
	float dotVal = MathSub::ClampFloat(m_forward.Dot(dir), -1.0f, 1.0f);
	float angle = std::acos(dotVal);

	// 軸
	Vector3 axis = m_forward.Cross(dir);
	if (axis.SqMagnitude() < 0.0f)//最小値
	{
		// 真逆の場合は安定した補助軸を使う
		if (std::fabs(m_forward.y) < 0.99f)
			axis = m_forward.Cross(Vector3::Up());
		else
			axis = m_forward.Cross(Vector3::Right());
	}
	axis = axis.Normalize();

	// クォータニオン
	m_rotaQ = Quaternion::AngleAxis(angle / m_rotaSpeed, axis);

	m_rotaFrame = m_rotaSpeed;
	m_nextForward = dir;
}

void Model::LookAt(Vector3 target)
{
	auto mat = Matrix4x4::LookAt(target);
	SetDir(mat * Vector3::Forward());
}


void Model::SetColor(float r, float g, float b, float a)
{
	if (m_modelHandle == -1)return;
	COLOR_F color = { r, g, b, a };
	color.r = MathSub::ClampFloat(color.r, 0.0f, 1.0f);
	color.g = MathSub::ClampFloat(color.g, 0.0f, 1.0f);
	color.b = MathSub::ClampFloat(color.b, 0.0f, 1.0f);
	color.a = MathSub::ClampFloat(color.a, 0.0f, 1.0f);
	DxLib::MV1SetDifColorScale(m_modelHandle, color);
	DxLib::MV1SetSpcColorScale(m_modelHandle, color);
	DxLib::MV1SetAmbColorScale(m_modelHandle, color);
}

void Model::SetColor(COLOR_F color)
{
	if (m_modelHandle == -1)return;
	color.r = MathSub::ClampFloat(color.r, 0.0f, 1.0f);
	color.g = MathSub::ClampFloat(color.g, 0.0f, 1.0f);
	color.b = MathSub::ClampFloat(color.b, 0.0f, 1.0f);
	color.a = MathSub::ClampFloat(color.a, 0.0f, 1.0f);
	DxLib::MV1SetDifColorScale(m_modelHandle, color);
	DxLib::MV1SetSpcColorScale(m_modelHandle, color);
	DxLib::MV1SetAmbColorScale(m_modelHandle, color);
}

void Model::SetEmiColor(float r, float g, float b, float a)
{
	if (m_modelHandle == -1)return;
	COLOR_F color = { r, g, b, a };
	color.r = MathSub::ClampFloat(color.r, 0.0f, 1.0f);
	color.g = MathSub::ClampFloat(color.g, 0.0f, 1.0f);
	color.b = MathSub::ClampFloat(color.b, 0.0f, 1.0f);
	color.a = MathSub::ClampFloat(color.a, 0.0f, 1.0f);
	MV1SetEmiColorScale(m_modelHandle, color);
}

void Model::SetEmiColor(COLOR_F color)
{
	if (m_modelHandle == -1)return;
	color.r = MathSub::ClampFloat(color.r, 0.0f, 1.0f);
	color.g = MathSub::ClampFloat(color.g, 0.0f, 1.0f);
	color.b = MathSub::ClampFloat(color.b, 0.0f, 1.0f);
	color.a = MathSub::ClampFloat(color.a, 0.0f, 1.0f);
	DxLib::MV1SetEmiColorScale(m_modelHandle, color);
}

void Model::SetMatrix(Matrix4x4 mat)
{
	if (m_modelHandle == -1)return;
	DxLib::MV1SetMatrix(m_modelHandle, mat.ToDxLibMATRIX());
}

void Model::ResetColor()
{
	COLOR_F color = { 1,1,1,1 };
	DxLib::MV1SetDifColorScale(m_modelHandle, color);
	DxLib::MV1SetSpcColorScale(m_modelHandle, color);
	DxLib::MV1SetAmbColorScale(m_modelHandle, color);
}

void Model::SetModelHandle(int modelHandle)
{
	//モデルの削除
	End();
	//新しいモデル
	m_modelHandle = modelHandle;
}

Vector3 Model::GetDir()
{
	Vector3 dir = m_forward;
	if (dir.Magnitude() > 0.0f)
	{
		dir = dir.Normalize();
	}
	return dir;
}

void Model::ModelHit()
{

}

void Model::SetAnim(const char* animName, bool isLoop)
{
	if (m_modelHandle == -1)return;

	auto animNameT = StringUtil::ToTCHAR(animName);
	//アニメーションを変更
	m_animator->SetAnim(m_modelHandle, DxLib::MV1GetAnimIndex(m_modelHandle, animNameT), isLoop);
}

void Model::SetAnim(const char* animName, bool isLoop, const float& animSpeed)
{
	if (m_modelHandle == -1)return;

	auto animNameT = StringUtil::ToTCHAR(animName);
	//アニメーションを変更
	m_animator->SetAnim(m_modelHandle, DxLib::MV1GetAnimIndex(m_modelHandle, animNameT), isLoop, animSpeed);
}

void Model::SetAnimSpeed(const float& animSpeed)
{
	//アニメーションの速度
	m_animator->SetAnimSpeed(animSpeed);
}

bool Model::IsFinishAnim()
{
	//アニメーションが終わったかどうか
	return m_animator->IsFinishAnim();
}

float Model::GetTotalAnimFrame()
{
	//全体フレーム
	return m_animator->GetTotalAnimFrame();
}

void Model::SetTotalAnimFrame(float totalFrame)
{
	//全体フレームセット
	m_animator->SetTotalAnimFrame(totalFrame);
}

float Model::GetNowAnimFrame()
{
	//現在のフレーム
	return m_animator->GetAnimFrame();
}

void Model::ReplayAnim()
{
	//アニメーションを再生
	m_animator->Replay();
}

void Model::DeleteAnim()
{
	if (m_modelHandle == -1)return;

	//削除
	m_animator->AllRemoveAnim(m_modelHandle);
}

void Model::SetFixedLoopFrame(float loopFrame)
{
	//指定ループ
	m_animator->SetFixedLoop(loopFrame);
}

bool Model::IsFinishFixedLoop()
{
	//指定ループ終了
	return m_animator->IsFinishFixedLoop();
}
void Model::SetTimeScale(float scale)
{
	m_animator->SetTimeScale(scale);
	m_timeScale = scale;
}
void Model::ApplyMat()
{
	if (m_modelHandle == -1)return;

	Matrix4x4 mat;
	auto pMat = Matrix4x4::TranslateMat4x4(m_pos.x, m_pos.y + m_modelHeightAdjust, m_pos.z);
	auto rMat = m_rotation.GetMatrix();
	auto sMat = Matrix4x4::ScaleMatrix4x4(m_scale);
	mat = sMat * rMat * pMat;
	DxLib::MV1SetMatrix(m_modelHandle, mat.ToDxLibMATRIX());
}