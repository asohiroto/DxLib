#pragma once
#include "../UIBase.h"
#include "../../../General/Math/MyMath.h"
#include <vector>
class Fader;
class MissionBoardUI;
class MissionBoardUIData;
class SelectUI :
	public UIBase
{
public:
	SelectUI();
	~SelectUI();
	//更新
	void Update() override;
	//描画
	void Draw()const override;

	//選んでるメニューのインデックスを設定
	void SetSelectMainMenuIndex(int index);
	void SetSelectTutorialMenuIndex(int index);
	void SetSelectStageMenuIndex(int index);
private:

	//メニューUI
	struct MenuUI
	{
		int handle = -1;
		Vector2 pos;
	};

	//メインメニューの項目
	std::vector<MenuUI> m_mainMenus;
	//選んでるメニューのインデックス
	int m_selectMainMenuIndex;

	//チュートリアルメニューの項目座標
	std::vector<MenuUI> m_tutorialMenus;
	//選んでるメニューのインデックス
	int m_selectTutorialMenuIndex;

	//ステージメニューの項目座標
	std::vector<MenuUI> m_stageMenus;
	std::weak_ptr<MissionBoardUI> m_stageMissionBoard;
	std::weak_ptr<MissionBoardUI> m_tutorialMissionBoard;
	//選んでるメニューのインデックス
	int m_selectStageMenuIndex;

	//影
	int m_shadowMenuHandle;
	int m_shadowCursorHandle;

	//カーソル
	MenuUI m_cursor;
	float m_cursorAngle;

	//状態に合わせた更新
	using UpdateFunc = void(SelectUI::*)();
	UpdateFunc m_update;

private:

	//メインメニューの更新
	void UpdateMainMenu();
	//チュートリアルメニューの更新
	void UpdateTutorialMenu();
	//ステージメニューの更新
	void UpdateStageMenu();

	//メニュー描画
	void DrawMenu(Fader& fader, const std::vector<MenuUI>& menuPos, int selectMeueIndex, bool isMenuMode) const;
};

