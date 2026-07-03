#pragma once
#include "../UIBase.h"
#include <vector>
class EndingTextUIData;
class TitleUI;
class EndingTextUI :
    public UIBase
{
public:
	EndingTextUI();
	~EndingTextUI();
	//更新
	void Update() override;
	//描画
	void Draw()const override;
	//全ての処理が終了したか
	bool IsAllEnd()const { return m_titleAlpha >= 255.0f; };
private:
	//データ
	std::vector<std::shared_ptr<EndingTextUIData>> m_endingDatas;

	//フォントハンドル
	std::vector<int> m_fontDatas;

	//スクロールオフセット値
	std::vector<float> m_scrollOffsetYs;
	//スクロール位置
	float m_scrollY;

	//黒背景透明度
	float m_blackAlpha;

	//タイトル透明度
	float m_titleAlpha;
	//タイトルUI
	int m_titleHandle;
};

