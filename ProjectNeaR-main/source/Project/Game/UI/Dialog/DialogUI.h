#pragma once
#include "../UIBase.h"
#include "../../../General/Math/MyMath.h"
#include <string>

class DialogUI :
    public UIBase
{
public:
    DialogUI(std::wstring text);
    ~DialogUI();
    void Update()override;
    void Draw()const override;

    void Yes() { m_isYes = true; }
    void No() { m_isYes = false; }
    bool IsYes()const { return m_isYes; };
private:

    //ハンドル
    int m_yesHandle;
    int m_noHandle;
    int m_backHandle;
    int m_cursorHandle;
    int m_backShadowHandle;
    int m_textHandle;

    //テキスト
    std::wstring m_text;

    //カーソル座標
    Vector2 m_cursorPos;
    
    //「はい」か「いいえ」
    bool m_isYes;

    //すぐに選択できないよう数フレーム入力を無効
    int m_countFrame;
};

