#pragma once
//フェードインとフェードアウトをするクラス
class Fader final
{
private:
	//シングルトンの準備
	Fader() = default;
	~Fader() = default;
	//コピー禁止
	Fader(const Fader&) = delete;
	Fader& operator = (const Fader&) = delete;
	//ムーブ禁止
	Fader(Fader&&) = delete;
	Fader& operator = (Fader&&) = delete;
public:
	/// <summary>
	/// シングルトンインスタンスを取得する
	/// </summary>
	/// <returns></returns>
	static Fader& GetInstance()
	{
		static Fader instance;
		return instance;
	}
	//初期化
	void Init();
	//更新
	void Update();
	//描画
	void Draw()const;
	//だんだん明るくなる
	void FadeIn(float speed = 10.0f);
	//だんだん暗くなる
	void FadeOut(float speed = 10.0f);
	//フェードインが終了したか
	bool IsFinishFadeIn()const;
	//フェードアウトが終了したか
	bool IsFinishFadeOut()const;
	//フェード中か
	bool IsFadeNow()const { return m_isFadeNow; };
	bool IsFadeInNow()const { return m_isFadeIn; };
	bool IsFadeOutNow()const { return m_isFadeOut; };

	//色変更
	void SetColor(unsigned int color) { m_color = color; };
private:
	//だんだん明るくなる
	bool m_isFadeIn = false;
	//だんだん暗くなる
	bool m_isFadeOut = false;
	//フレームカウント
	float m_countFrame = 0.0f;
	//フェード速度
	float m_fadeSpeed = 0.0f;
	//フェード中
	bool m_isFadeNow = false;

	//色
	unsigned int m_color = 0x000000;
};

