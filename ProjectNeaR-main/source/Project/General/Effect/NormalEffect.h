#pragma once
#include "../Math/MyMath.h"
class NormalEffect
{
public:
	NormalEffect(int playHandle, Vector3 pos);
	virtual ~NormalEffect();
	virtual void Update();			//更新
	virtual void End();				//終了処理
	void Play();					//再生
	void Stop();					//停止
	void EnableDraw() { m_isDraw = true; };	//描画する
	void DisableDraw() { m_isDraw = false; };//描画しない
	void SetPlaySpeed(float speed);	//再生速度
	void Delete();					//削除
	bool IsDelete() const { return m_isDelete; };
	bool IsDraw() const { return m_isDraw; };
	int GetPlayHandle() const { return m_playHandle; };
	//セッター
	void SetPos(Vector3 pos);		//座標
	void SetRot(Vector3 rot);		//回転
	void SetScale(Vector3 scale);	//拡大
	void LookAt(Vector3 dir);		//向きたい方向
	//ゲッター
	Vector3 GetPos()const { return m_pos; };
	Vector3 GetRot()const { return m_rot; };
	Vector3 GetScale()const { return m_scale; };

	//指定フレーム再生
	void SpecificFrame(float frame);

	//タイムスケール
	void SetTimeScale(float scale);
	//自分のタイムスケールを使う
	void EnableIsMyScale();
	//使わない
	void DisableIsMyScale();
protected:
	//再生ハンドル
	int m_playHandle;
	//座標
	Vector3 m_pos;
	//回転
	Vector3 m_rot;
	//拡大
	Vector3 m_scale;
	//再生速度
	float m_playSpeed;
	//削除フラグ
	bool m_isDelete;
	//指定フレーム後に削除
	float m_specificFrame;
	bool m_isSpecificFrame;
	//エフェクトの向き(Z方向)
	Vector3 m_dir;
	//タイムスケール
	float m_myTimeScale;
	bool m_isMyTimeScale;

	//描画
	bool m_isDraw;
};

