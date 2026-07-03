#pragma once
class SceneController;

/// <summary>
/// 各シーンの基底クラス
/// </summary>
class SceneBase
{
protected:
	//各シーンの状態を切り替えるため
	SceneController& m_controller;
public:
	SceneBase(SceneController& controller);
	/// <summary>
	/// 初期化処理
	/// </summary>
	virtual void Init() abstract;
	/// <summary>
	/// 内部変数の更新
	/// </summary>
	/// <param name="input"></param>
	virtual void Update() abstract;
	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw() abstract;
	/// <summary>
	/// 終了処理
	/// </summary>
	virtual void End() abstract;
	/// <summary>
	/// 再スタート処理
	/// </summary>
	virtual void Restart() abstract;
	//デバッグ用
	virtual void DebugDraw() const abstract;
};

