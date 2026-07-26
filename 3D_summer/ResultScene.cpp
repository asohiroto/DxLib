#include "ResultScene.h"
#include "SceneManager.h"
#include "Input.h"
#include "GameDefine.h"
#include <DxLib.h>

using namespace GameDefine;

ResultScene::ResultScene() :
	_winner(0),
	p_Manager(nullptr),
	_modelH(-1),
	_skyDomeH(-1),
	_modelPos(VGet(500.0f, 0.0f, 10.0f)),
	_attachAnimIndex(-1),
	_animCount(0.0f)
{
}

ResultScene::~ResultScene()
{
}

void ResultScene::Init(SceneManager* pManager, int winner, int modelH, int skyDomeH)
{
	_winner = winner;
	p_Manager = pManager;

	if (_winner == 0)
	{
		DrawString(0, 0, "ERROR!!", 0xff0000);
	}

	_modelH = modelH;
	_skyDomeH = skyDomeH;

	MV1SetScale(_modelH, VGet(3.0f, 3.0f, 3.0f));
	MV1SetPosition(_modelH, _modelPos);

	MV1SetScale(_skyDomeH, VGet(5.0f, 5.0f, 5.0f));
	MV1SetPosition(_skyDomeH, VGet(0.0f, 0.0f, 0.0f));

	if (_winner == 1)
	{
		_attachAnimIndex = MV1AttachAnim(_modelH, 1, -1, true);
	}
	else if (_winner == 2)
	{
		_attachAnimIndex = MV1AttachAnim(_modelH, 4, -1, true);
	}
	_animCount = 0;
}

void ResultScene::Update(std::shared_ptr<Input> pInput)
{
	if (_attachAnimIndex != -1)
	{
		float totalTime = MV1GetAttachAnimTotalTime(_modelH, _attachAnimIndex);
		_animCount += 0.5f;
		if (_animCount >= totalTime) _animCount = 0.0f;
		MV1SetAttachAnimTime(_modelH, _attachAnimIndex, _animCount);
	}

}

void ResultScene::Draw()
{
	SetDrawArea(0, 0, WIDTH, HEIGHT);
	SetCameraScreenCenter(WIDTH / 2, HEIGHT);
	SetupCamera_Perspective(DX_PI_F / 4.0f);
	SetCameraPositionAndTargetAndUpVec(
		VGet(_modelPos.x, 300.0f, _modelPos.z - 700.0f),
		VGet(_modelPos.x, 150.0f, _modelPos.z),
		VGet(0.0f, 1.0f, 0.0f)
	);

	MV1DrawModel(_skyDomeH);
	MV1DrawModel(_modelH);

	DrawFormatString(300, 300, 0xffffff, "Player : %d  WIN!!", _winner);

}
