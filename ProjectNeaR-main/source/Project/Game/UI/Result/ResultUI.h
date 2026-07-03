#pragma once
#include "../UIBase.h"
#include <memory>
#include <string>
class Timer;
class ResultUI :
    public UIBase
{
public:
    ResultUI(std::wstring stageName,std::shared_ptr<Timer> timer);
    ~ResultUI();
    //更新
    void Update() override;
    //描画
    void Draw()const override;
    //描画終了
    bool IsFinish()const;
private:
    struct Image
    {
        int handle = -1;
        int alpha = 0;
    };

    //リザルト
    Image m_resultHandle;
    //リザルト情報
    Image m_resultInfoHandle;
    //PressAnyBotton
    Image m_pressAnyBottonHandle;
    //背景
    int m_backHandle;

    //フォント
    int m_fontHandle;
private:
    //フレーム
    int m_countFrame;
    //タイム
    std::shared_ptr<Timer> m_timer;
    //ランク
    std::wstring m_rank;
    //時間テキスト
    std::wstring m_timeText;
};

