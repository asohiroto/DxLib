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
	TurnState GetNowTurn() const { return _nowTurn; }
	// 現在のターン数を取得するゲッター
	int GetTurnCount() const { return _turnCount; }

public:
	// 現在のターン数
	int _turnCount;

private:
	// 現在のターンを表示する関数
	void DrawTurnState();

private:
	// 現在のターン
	TurnState _nowTurn;
};