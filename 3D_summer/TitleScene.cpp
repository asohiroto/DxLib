#include "TitleScene.h"
#include "SceneManager.h"
#include "Input.h"
#include <DxLib.h>

TitleScene::TitleScene() :
	p_Manager(nullptr),
	_titlePosX(100),
	_titlePosY(100),
	_nextPosX(700),
	_nextPosY(500),
	_logoH(-1),
	_modelH(-1),
	_modelSubH(-1),
	_skyDomeH(-1),
	_titleAnimIndex(-1),
	_animCount(0.0f),
	_subAnimIndex(-1),
	_subCount(0.0f),
	_modelPos(VGet(1000.0f, 0.0f, 10.0f)),
	_subPos(VGet(300.0f, 0.0f, 10.0f))
{
}

TitleScene::~TitleScene()
{
}

void TitleScene::Init(SceneManager* pManager, int modelH, int modelSubH, int skyDomeH)
{
	p_Manager = pManager;

	_logoH = LoadGraph("data/title.png");
	_modelH = modelH;
	_modelSubH = modelSubH;
	_skyDomeH = skyDomeH;

	MV1SetScale(_skyDomeH, VGet(5.0f, 5.0f, 5.0f));
	MV1SetPosition(_skyDomeH, VGet(0.0f, 0.0f, 0.0f));

	MV1SetScale(_modelH, VGet(3.0f, 3.0f, 3.0f));
	MV1SetPosition(_modelH, _modelPos);

	MV1SetScale(_modelSubH, VGet(3.0f, 3.0f, 3.0f));
	MV1SetPosition(_modelSubH, _subPos);

	_titleAnimIndex = MV1AttachAnim(_modelH, 3, -1, false);
	_animCount = 0.0f;

	_subAnimIndex = MV1AttachAnim(_modelSubH, 0, -1, false);
	_animCount = 0.0f;
}

void TitleScene::Update(std::shared_ptr<Input> pInput)
{
	if (_titleAnimIndex != -1)
	{
		float totalTime = MV1GetAttachAnimTotalTime(_modelH, _titleAnimIndex);
		_animCount += 0.5f;
		if (_animCount >= totalTime) _animCount = 0.0f;
		MV1SetAttachAnimTime(_modelH, _titleAnimIndex, _animCount);
	}

	if (_subAnimIndex != -1)
	{
		float totalSubTime = MV1GetAttachAnimTotalTime(_modelSubH, _subAnimIndex);
		_subCount += 0.5f;
		if (_subCount >= totalSubTime) _subCount = 0.0f;
		MV1SetAttachAnimTime(_modelSubH, _subAnimIndex, _subCount);
	}


	if (pInput->IsTrigger(PAD_INPUT_A))
	{
		if (_titleAnimIndex != -1 && _subAnimIndex != -1)
		{
			MV1DetachAnim(_modelH, _titleAnimIndex);
			_titleAnimIndex = -1;

			MV1DetachAnim(_modelSubH, _subAnimIndex);
			_subAnimIndex = -1;
		}
		p_Manager->ChangeScene(SceneManager::SceneName::GAME, 0);


	}
}

void TitleScene::Draw()
{
	MV1DrawModel(_skyDomeH);

	// ステージの床を描画
	DrawCube3D
	(
		VGet(-(GRID_NUM / 2 * GRID_SIZE) + 500.0f, -20.0f, -(GRID_NUM / 2 * GRID_SIZE) + 500.0f),
		VGet((GRID_NUM / 2 * GRID_SIZE) + 500.0f, 0, (GRID_NUM / 2 * GRID_SIZE) + 500.0f),
		0xffffff, 0xffffff, true
	);

	MV1DrawModel(_modelH);
	MV1DrawModel(_modelSubH);

	DrawGraph(_titlePosX, _titlePosY, _logoH, true);

	// UI描画
	DrawString(_nextPosX, _nextPosY, "Press A Button", 0xffffff, 0xffff00);
}