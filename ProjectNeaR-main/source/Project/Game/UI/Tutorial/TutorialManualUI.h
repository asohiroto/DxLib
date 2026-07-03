#pragma once
#include "../UIBase.h"
#include "../../../General/StageIndex.h"
#include "../../../General/Math/MyMath.h"
class TutorialManualUI :
    public UIBase
{
public:
    TutorialManualUI(StageIndex index);
    ~TutorialManualUI();
    //更新
    void Update() override;
    //描画
    void Draw()const override;
private:
    //ハンドル
    int m_handle;
    //座標
    Vector2 m_pos;
};

