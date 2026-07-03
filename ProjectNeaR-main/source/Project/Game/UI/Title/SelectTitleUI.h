#pragma once
#include "../UIBase.h"
#include "../../../Scene/TitleScene.h"
#include "../../../General/Math/MyMath.h"
#include <vector>

class SaveDataUI;
class SelectTitleUI :
    public UIBase
{
public:
	SelectTitleUI();
	~SelectTitleUI();
	//更新
	void Update() override;
	//描画
	void Draw()const override;

	//メニューセレクト
	void SetMenuIndex(TitleScene::SelectMenuTitle index) { m_selectIndex = index; };
private:
	//ハンドル
	std::vector<int> m_menuHandle;
	//座標
	std::vector<Vector2> m_menuPoses;

	//影
	int m_shadowMenuHandle;
	int m_shadowCursorHandle;

	//カーソル
	int m_cursorHandle;
	Vector2 m_cursorPos;

	//セレクト中の番号
	TitleScene::SelectMenuTitle m_selectIndex;

	//セーブデータUI
	std::weak_ptr<SaveDataUI> m_saveDataUI;
};

