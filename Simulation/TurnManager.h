#pragma once

class TurnManager
{
public:
	// ターンの状態
	enum class TurnState
	{
		PlayerSelectTurn,
		SelectResultTurn,
		EnemyTurn
	};

public:
	TurnManager();
	~TurnManager();
	void Init();
	void Update();
	void Draw();

	// ターンを変更する関数
	void TurnChange();
	// 現在のターンを取得するゲッター
	TurnState GetNowTurn() const {return _nowTurn ; }

private:
	// 現在のターンを表示する関数
	void DrawTurnState();

private:
	// 現在のターン
	TurnState _nowTurn;
};