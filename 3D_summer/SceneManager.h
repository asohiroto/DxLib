#pragma once
#include<memory>

class TitleScene;
class SceneGame;

class SceneManager
{
public:
	enum SceneName
	{
		TITLE,
		GAME,
		RESULT
	};

	SceneManager();
	~SceneManager();
	void Init();
	void Update();
	void Draw();

	void ChangeScene(int name);

private:
	// 現在のシーン
	int _nowScene;

	std::shared_ptr<TitleScene> p_Title;

	std::shared_ptr<SceneGame> p_Game;
};

