#pragma once

class AnimManager
{
public:
	AnimManager();
	virtual ~AnimManager();
	void Init(int handle);
	void End();
	void Update();
	void Draw();
	// アニメーションを変更する
	void AnimChange(int animInd, bool isLoop, int playFrame);

private:
	// このクラスで参照するモデルのハンドル
	int _modelH;
	// 現在再生中のアニメーションの番号
	int _nowAnimInd;
	// アニメーションの総再生時間
	float _totalAnimTime;
	// 再生中のフレーム
	float _playTime;
	// 再生速度
	float _playSpeed;
	// アタッチされているアニメーションの識別子
	int _attachInd;
	// ループ再生を行うか
	bool _isLoop;
	// アニメーションが終了したか
	bool _isFinished;
};