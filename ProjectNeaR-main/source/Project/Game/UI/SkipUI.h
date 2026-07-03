#pragma once
#include "UIBase.h"
#include <memory>
class BottunUI;
class SkipUI :
    public UIBase
{
public:
	SkipUI();
	~SkipUI();
	//更新
	void Update() override;
	//描画
	void Draw()const override;

	//割合セット
	void SetRate(float rate);

private:
	//割合
	float m_rate;
	//ハンドル
	int m_handle;

	//フォントハンドル
	int m_fontHandle;

	//ボタン
	std::shared_ptr<BottunUI> m_bottunUI;

	//透明度
	int m_alpha;
};

