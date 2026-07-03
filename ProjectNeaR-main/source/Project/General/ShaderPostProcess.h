#pragma once
class ShaderPostProcess
{
public:
	ShaderPostProcess();
	~ShaderPostProcess();
	void Init();
	void Update();
	void Draw(const int rt)const;
	void Draw(float x,float y, const int handle)const;
	void End();
	//ポストエフェクトの状態
	enum class PostEffectState : int
	{
		Normal = 0,
		Glitch = 1 << 0, //0001
		Gray = 1 << 1,   //0010
		JustAvoid = 1 << 2, //0100
		NoColorGlitch = 1 << 3, //1000
	};
	//ポストエフェクトの状態を設定
	void SetPostEffectState(const PostEffectState state){ m_cbuff1->state = static_cast<int>(state); }
	//状態の加算
	void AddPostEffectState(const PostEffectState state) { m_cbuff1->state |= static_cast<int>(state); }
	//状態の減算
	void SubPostEffectState(const PostEffectState state) { m_cbuff1->state &= ~static_cast<int>(state); }
	//状態リセット
	void ResetPostEffectState() { m_cbuff1->state = static_cast<int>(PostEffectState::Normal); }
	//ジャスト回避エフェクトの時間を設定
	void SetJustAvoidEffectTime(const float time);

	//グリッジのブロックノイズの大きさ
	void SetBlockScele(float scale);
	//ノイズの速度
	void SetNoiseSpeed(float speed);
	//揺れの強さ
	void SetShakeStrength(float shake);

	//グリッジ用の定数バッファ
	struct CBuff1
	{
		float shakeStrength;	//揺れの強さ(でかいほどバグったような表現が強くなる)
		float blockScale;		//ブロックノイズの大きさ
		float noiseSpeed;		//ノイズの動く速さ
		int state; 
	};
	//ジャスト回避用の定数バッファ
	struct CBuff2
	{
		float justRate;		//エフェクトの時間
		float dummy1 = 0.0f;
		float dummy2 = 0.0f;
		float dummy3 = 0.0f;
	};
private:
	//定数バッファハンドル
	int m_cbuff1Handle;
	int m_cbuff2Handle;
	//定数バッファのポインタ
	CBuff1* m_cbuff1;
	CBuff2* m_cbuff2;

	//設定用バッファ
	CBuff1 m_optionCbuff;

	//シェーダハンドル
	int m_psHandle;

	//ジャスト回避フレーム
	float m_justFrame;
	float m_countJustFrame;
};

