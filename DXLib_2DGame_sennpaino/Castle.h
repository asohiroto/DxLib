#pragma once
#include "Character.h"

class Castle : public Character
{
public :
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Castle();

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~Castle();
	
	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Init() override;
	
	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() override;
	
	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw();

private:

};