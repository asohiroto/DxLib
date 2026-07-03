#pragma once
#include "UIBase.h"
#include <string>
#include <list>
#include <memory>
class TextData;
class TalkUI :
    public UIBase
{
public:
    TalkUI(std::list<std::shared_ptr<TextData>> textDatas);
    ~TalkUI();
    //更新
    void Update() override;
    //描画
    void Draw()const override;
private:
    //後ろ
    int m_backHandle;
    //フォント
    int m_fontHandle;
private:
    //テキスト
    std::wstring m_text;
    //話してる人
    std::wstring m_speaker;
    //表示する文字数
    int m_displayTextNum;
    //フレーム
    int m_countFrame;
    //話すスピード
    int m_talkSpeed;
    //テキストデータ
    std::list<std::shared_ptr<TextData>> m_textDatas;
    //画像ハンドル
    std::list<int> m_imageHandles;
};

