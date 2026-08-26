#include "AnimManager.h"
#include <DxLib.h>

AnimManager::AnimManager() :
	_modelH(-1), _nowAnimInd(-1),
	_totalAnimTime(0.0f), _playTime(0)
{
}

AnimManager::~AnimManager()
{
}

void AnimManager::Init(int handle)
{
	_modelH = handle;
}

void AnimManager::End()
{
}

void AnimManager::Update()
{
}

void AnimManager::Draw()
{
}

void AnimManager::AnimChange(int animInd, bool isLoop, int playFrame)
{
	if (_nowAnimInd == animInd) return;

	if (_attachInd != -1) MV1DetachAnim(_modelH, _attachInd);

	_attachInd = MV1AttachAnim(_modelH, animInd, -1, false);
	_totalAnimTime = MV1GetAnimTotalTime(_modelH, _nowAnimInd);

	_playSpeed = _totalAnimTime / static_cast<float>(playFrame);

	_playTime = 0.0f;

}
