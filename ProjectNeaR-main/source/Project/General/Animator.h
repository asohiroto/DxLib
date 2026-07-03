#pragma once
class Animator
{
private:
	struct Anim
	{
		int m_attachAnim = -1;//再生中のアニメーション名
		int m_attachAnimIndex = -1;//アニメーションのアタッチされた番号
		float m_animTimer = 0.0f;//アニメーションのタイマー
		float m_animStopTime = 0.0f;//アニメーションの終了タイマー
		bool m_isLoopAnim = false;//ループするアニメーションかどうか
		bool m_isFinishAnim = false;//アニメーションが終わったらtrue
		float m_animSpeed = 0.5f;//再生速度
		//一定フレーム間ループ再生
		bool m_isFixedLoop = false;
		float m_fixedLoopFrame = 0.0f;
	};
	Anim m_animNow;		//今のアニメーション
	Anim m_animNext;	//次のアニメーション
	//ブレンドに使う
	float m_blendRate;
	//タイムスケール
	float m_timeScale;
	//アニメーションを進める
	void UpdateAnim(const int& modelHandle, Anim& anim);
	//ブレンド率の更新
	void UpdateBlend(const int& modelHandle);
public:
	//コンストラクタ
	Animator();
	//デストラクタ
	~Animator();

	/// <summary>
	/// アニメーションのアタッチ
	/// </summary>
	/// <param name="modelHandle">モデルのハンドル</param>
	/// <param name="anim">animation enum</param>
	/// <param name="isLoop">ループするか</param>
	void SetAnim(const int& modelHandle, const int& anim, const bool& isLoop);

	//アニメーションのアタッチと再生速度変更
	void SetAnim(const int& modelHandle, const int& anim, const bool& isLoop,const float& animSpeed);

	/// <summary>
	/// 最初から再生
	/// </summary>
	void Replay();

	/// <summary>
	/// 指定されたアニメーションが再生中なら削除
	/// </summary>
	/// <param name="modelHandle">モデルのハンドル</param>
	/// <param name="anim">animation enum</param>
	void RemoveAnim(const int& modelHandle, Anim& anim);

	/// <summary>
	/// アニメーションを全て削除
	/// </summary>
	/// <param name="modelHandle"></param>
	void AllRemoveAnim(const int& modelHandle);

	/// <summary>
	/// アニメーションの再生
	/// </summary>
	/// <param name="modelHandle"></param>
	void PlayAnim(const int& modelHandle);

	/// <summary>
	/// アニメーションが終わったかどうか
	/// </summary>
	/// <returns></returns>
	bool IsFinishAnim();

	//アニメーションが終わるフレーム(全体フレーム / 再生速度)
	float GetTotalAnimFrame() { return  m_animNext.m_animStopTime / m_animNext.m_animSpeed; };
	//アニメフレーム(フレーム / 再生速度)
	float GetAnimFrame() { return  m_animNext.m_animTimer / m_animNext.m_animSpeed; };
	//アニメーションの全体フレームをセットする
	void SetTotalAnimFrame(float totalFrame) { m_animNext.m_animStopTime = totalFrame; };
	//ループフレームをセットする
	void SetFixedLoop(float loopFrame);
	//ループ再生が終了したか
	bool IsFinishFixedLoop();
	//アニメーションの速度を変更
	void SetAnimSpeed(const float& animSpeed);
	//タイムスケール
	void SetTimeScale(float timeScale) { m_timeScale = timeScale; };
};
