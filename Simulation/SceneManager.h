#pragma once
#include<memory>
#include<list>

class SceneBase;

class SceneManager
{
public:
	SceneManager();
	~SceneManager();
	void Init();
	void Update();
	void Draw();
	// シーン遷移用関数
	void ChangeScene(std::shared_ptr<SceneBase> scene);
	// リストにシーンを入れる関数
	void PushScene(std::shared_ptr<SceneBase> scene);
	// リストのシーンを取り出す関数
	void PopScene(std::shared_ptr<SceneBase> scene);

private:
	// スタック構造のシーンのリスト
	using SceneStack_T = std::list<std::shared_ptr<SceneBase>>;
	// 現在のシーンのポインタ
	SceneStack_T _nowScene;
	std::shared_ptr<SceneBase> _nextScene;
};

