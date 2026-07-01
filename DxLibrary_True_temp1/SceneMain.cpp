#include "SceneMain.h"
#include"DxLib.h"

SceneMain::SceneMain():
	_frameCount(0)
{

}

SceneMain::~SceneMain()
{

}

void SceneMain::Init()
{
	player.Init();
	_frameCount = 0;
}

void SceneMain::End()
{

}

void SceneMain::Update()
{
	player.Update();
	_frameCount++;
}

void SceneMain::Draw()
{
	player.Draw();

	DrawString(0, 0, "SceneMain", 0xffffff);
	DrawFormatString(0, 20,  0xffffff, "TotalFrame : %d", _frameCount);
}

