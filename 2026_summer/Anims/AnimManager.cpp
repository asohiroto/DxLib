#include "AnimManager.h"
#include <DxLib.h>

AnimManager::AnimManager() :
	_modelH(-1), _nowAnimInd(-1),
	_totalAnimTime(0.0f), _playTime(0.0f),
	_playSpeed(0.0f), _attachInd(-1), _isLoop(false),
	_isFinished(false)
{
}

AnimManager::~AnimManager()
{
	if (_attachInd != -1)
	{
		MV1DetachAnim(_modelH, _attachInd);
		_attachInd = -1;
	}
}

void AnimManager::Init(int handle)
{
	_modelH = handle;
}

void AnimManager::End()
{
	if (_attachInd != -1)
	{
		MV1DetachAnim(_modelH, _attachInd);
		_attachInd = -1;
	}
}

void AnimManager::Update()
{
	// アニメーション未アタッチなら何もしない
	if (_attachInd == -1) return;

	_playTime += _playSpeed;

	// 再生時間が総時間に達したらループ再生するか終了扱いにする
	if (_playTime >= _totalAnimTime)
	{
		if (_isLoop) _playTime = 0.0f;
		else
		{
			_playTime = _totalAnimTime;
			_isFinished = true;
		}
	}
	MV1SetAttachAnimTime(_modelH, _attachInd, _playTime);
}

void AnimManager::Draw()
{
}

void AnimManager::AnimChange(AnimInfo info)
{
	// 同じアニメーションなら変更しない
	if (_nowAnimInd == info.animInd) return;

	// 現在アタッチ中のアニメーションを解除
	if (_attachInd != -1) MV1DetachAnim(_modelH, _attachInd);

	_attachInd = MV1AttachAnim(_modelH, info.animInd, -1, false);
	_totalAnimTime = MV1GetAttachAnimTotalTime(_modelH, _attachInd);

	// 指定フレーム数で再生し終わるように再生速度を算出
	_playSpeed = _totalAnimTime / static_cast<float>(info.playFrame);

	_playTime = 0.0f;
	_nowAnimInd = info.animInd;
	_isLoop = info.isLoop;
	_isFinished = false;

}