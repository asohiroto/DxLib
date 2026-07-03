#pragma once
#include "../UIBase.h"
#include "../../../General/Math/MyMath.h"
#include <string>
class BottunUI :
    public UIBase
{
public:
	BottunUI(std::wstring bottun);
	~BottunUI();
	//更新
	void Update() override;
	//描画
	void Draw()const override;
	//描画座標セット
	void SetPos(const Vector2& pos) { m_pos = pos; };
public:
	//ボタン
	std::wstring m_wBottun;
	std::string m_sBottun;
	//ボタンハンドル
	int m_noPushBottunHandle;
	int m_pushBottunHandle;
	//描画座標
	Vector2 m_pos;
};

