#pragma once
#include "../UIBase.h"
#include "../../../General/Math/MyMath.h"
#include <map>
#include <vector>
class MissionBoardUIData;
class MissionBoardUI :
	public UIBase
{
public:
	MissionBoardUI(int size,std::vector<std::shared_ptr<MissionBoardUIData>> datas,bool isTutorial = false);
	~MissionBoardUI();
	//更新
	void Update() override;
	//描画
	void Draw()const override;
	//メニューセレクト
	void SetSelectMissionBoardIndex(int index) { m_index = index; };
private:
	//ハンドル
	std::map<int, int> m_handles;
	int m_backHandle;
	int m_backShadowHandle;
	int m_clearMarkHandle;
	int m_scoreBoardHandle;
	int m_scoreBoardShadowHandle;

	//番号
	int m_index;

	//データ
	std::vector<std::shared_ptr<MissionBoardUIData>> m_datas;

	//フォントハンドル
	int m_textFontHandle;
	int m_titleFontHandle;
	int m_timeFontHandle;
	int m_rankFontHandle;
	int m_scoreFontHandle;

	//チュートリアルか
	bool m_isTutorial;
};

