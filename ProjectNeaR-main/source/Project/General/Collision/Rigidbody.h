#pragma once
#include "../Math/Vector3.h"
class Rigidbody
{
public:
	Rigidbody();
	virtual ~Rigidbody() {};
	//位置
	Vector3 GetPos() { return m_pos; };
	void SetPos(Position3 pos) { m_pos = pos; };
	void SetPosX(float x) { m_pos.x = x; };
	void SetPosY(float y) { m_pos.y = y; };
	void SetPosZ(float z) { m_pos.z = z; };
	//ベクトル
	Vector3 GetVec()const { return m_vec; };
	Vector3 GetVecWithTS()const;
	Vector3 GetMoveVec();//水平方向のベクトル
	void SetVec(Vector3 vec) { m_vec = vec; };
	void SetVecX(float x) { m_vec.x = x; };
	void SetVecY(float y) { m_vec.y = y; };
	void SetVecZ(float z) { m_vec.z = z; };
	void AddVec(Vector3 vec) { m_vec += vec; };
	void SetMoveVec(Vector3 vec);
	void ResetVec() { m_vec = m_vec.Zero(); };
	//次の座標
	Vector3 GetNextPos() const;
	//スピードダウン
	void SpeedDown(float decRate = 0.8f);
	//自分のタイムスケールを使うか
	void SetIsMyTimeScale(bool isUse) { m_isMyTimeScale = isUse; };
	bool IsMyTimeScale() const { return m_isMyTimeScale; };
	//自分のタイムスケールを設定
	void SetMyTimeScale(float timeScale) { m_myTimeScale = timeScale; };
	float GetMyTimeScale() const { return m_myTimeScale; };
	//重力
	void SetIsGravity(bool isGravity) { m_isGravity = isGravity; };
public:
	//座標とベクトル
	Vector3 m_pos;
	Vector3 m_vec;
	//重力を受ける
	bool m_isGravity ; //重力を受けるかどうか
	//自分のタイムスケールを優先する
	bool m_isMyTimeScale;
	//自分のタイムスケール
	float m_myTimeScale;
};

