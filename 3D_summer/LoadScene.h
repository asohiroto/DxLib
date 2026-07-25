#pragma once

class SceneManager;

class LoadScene
{
public:
	LoadScene();
	~LoadScene();
	void Init(SceneManager* pManager);
	void Update();
	void Draw();

	// 読み込んだハンドルを、遷移先のGameシーンが受け取れるようにする
	int GetPlayerModelH() const { return _playerModelH; }
	int GetPlayerSubModelH() const { return _playerSubModelH; }
	int GetSkyDomeH() const { return _skyDomeH; }

private:
	SceneManager* p_Manager;
	// 進捗表示用
	int _totalRequestNum;
	// 読み込んだ各種ハンドル
	int _playerModelH;
	int _playerSubModelH;
	int _skyDomeH;
};