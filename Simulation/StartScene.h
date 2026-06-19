#pragma once
#include"SceneBase.h"

/// <summary>
///  スタートシーンクラス
/// </summary>
class StartScene : public SceneBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	StartScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~StartScene() override;

	/// <summary>
	/// 初期化
	/// </summary>
	void Init() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;
};
