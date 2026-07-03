#pragma once
#include "../UIBase.h"
#include "../../../General/Math/MyMath.h"
#include <string>
class SaveDataUI :
    public UIBase
{
public:
    SaveDataUI();
    ~SaveDataUI();
    //更新
    void Update() override;
    //描画
    void Draw()const override;
private:
    //描画位置
    Vector2 m_pos;

    //背景ハンドル
    int m_backHandle;
    int m_backShadowHandle;

    //フォントハンドル
    int m_fontHandle;

    //任務進行度
    std::wstring m_clearRateText;

    //総プレイ時間
    std::wstring m_totalPlayTimeText;

    //最後に遊んだ日付
    std::wstring m_lastPlayDateText;
};

