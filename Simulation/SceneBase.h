#pragma once

/// <summary>
/// シーンの基礎クラス
/// </summary>
class SceneBase
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	SceneBase() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~SceneBase() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Init() {};

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() {};

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw() {};
};