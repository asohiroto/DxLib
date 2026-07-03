#include "SceneController.h"
#include <memory>
#include<cassert>
#include "TitleScene.h"
#include "DebugScene/DebugSelectScene.h"
#include "../General/Input.h"

SceneController::SceneController():
	m_isUpdate(true)
{
#if _DEBUG
	//一番最初のシーンだけは割り当てる
	//自分自身のインスタンスを渡してあげる
	ChangeScene(std::make_shared<DebugSelectScene>(*this));
#else
	//一番最初のシーンだけは割り当てる
	//自分自身のインスタンスを渡してあげる
	ChangeScene(std::make_shared<TitleScene>(*this));
#endif

}

void SceneController::Update()
{
	//更新をしないならreturn
	if (!m_isUpdate)return;
#if _DEBUG
	//デバッグ用
	auto& input = Input::GetInstance();
	if (input.IsTrigger("DebugScene"))
	{
		ChangeScene(std::make_shared<DebugSelectScene>(*this));
		return;
	}
#endif

	//最後にプッシュ(入れた)シーンのみ更新処理を行う(他のシーンは更新はストップ)
	m_scenes.back()->Update();
}

void SceneController::Draw()
{

	//古いシーンから描画されるので最後に入れたシーンが前に描画される
	for (auto& scenes : m_scenes)
	{
		scenes->Draw();
	}
}

void SceneController::ChangeScene(std::shared_ptr<SceneBase> scene)
{
	//空(最初の処理)
	if (m_scenes.empty())
	{
		m_scenes.emplace_back(scene);
	}
	else
	{
		//終了処理
		m_scenes.back()->End();
		//先頭のシーンが切り替わる
		m_scenes.back() = scene;
	}
	//初期化処理
	m_scenes.back()->Init();
}

void SceneController::ChangeBaseScene(std::shared_ptr<SceneBase> scene)
{
	//空の場合エラー
	assert(!m_scenes.empty());
	//終了処理
	m_scenes.front()->End();
	m_scenes.front() = scene;
	//初期化処理
	m_scenes.front()->Init();
}

void SceneController::PushScene(std::shared_ptr<SceneBase> scene)
{
	m_scenes.emplace_back(scene);
	m_scenes.back()->Init();	//初期化処理
}

void SceneController::PopScene()
{
	//実行するシーンがなくなるので許可しない
	if (m_scenes.size() <= 1)return;
	m_scenes.back()->End();	//終了処理
	m_scenes.pop_back();	//末尾を取り除く
}

void SceneController::RestartBaseScene()
{
	//再スタート
	m_scenes.front()->Restart();
}

void SceneController::UpdateBaseScene()
{
	//更新処理
	m_scenes.front()->Update();
}

#if _DEBUG
void SceneController::DebugDraw() const
{
	//デバッグ描画
	for (auto& scenes : m_scenes)
	{
		scenes->DebugDraw();
	}
}
#endif