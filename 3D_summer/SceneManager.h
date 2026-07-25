#pragma once
#include<memory>

class TitleScene;
class LoadScene;
class SceneGame;
class ResultScene;
class Input;

class SceneManager
{
public:
	enum SceneName
	{
		TITLE,
		LOAD,
		GAME,
		RESULT
	};

	SceneManager();
	~SceneManager();
	void Init();
	void Update(std::shared_ptr<Input> pInput, std::shared_ptr<Input> pInputSub);
	void Draw();

	void ChangeScene(int name, int winner);

private:
	// 現在のシーン
	int _nowScene;

	std::shared_ptr<TitleScene> p_Title;

	std::shared_ptr<LoadScene> p_Load;

	std::shared_ptr<SceneGame> p_Game;

	std::shared_ptr<ResultScene> p_Result;
};

