#pragma once
#include "../UIBase.h"
#include "../../../General/Math/MyMath.h"
#include <memory>
class ShaderPostProcess;
class TitleUI :
    public UIBase
{
public:
    TitleUI();
    ~TitleUI();
    //更新
    void Update() override;
    //描画
    void Draw()const override;
private:
    //タイトル名
    int m_titleNameHandle;
    //タイトル影
    int m_titleNameShadowHandle;
    //背景
    int m_titleBackHandle;
    //歯車
	int m_gearHandle;
    //時計
	int m_clockHandle;
    

    //シェーダ
	std::shared_ptr<ShaderPostProcess> m_shader;
    //不定期に揺れる
	int m_shakeCountFrame;

    //タイトルが完成するまでのフレーム
	int m_titleCompleteCountFrame;

    //時計の座標
	Vector2 m_clockPos;
    //歯車1の位置
	Vector2 m_gear1Pos;
	//歯車2の位置
	Vector2 m_gear2Pos;
	//歯車3の位置
	Vector2 m_gear3Pos;
	//歯車4の位置
	Vector2 m_gear4Pos;
    //タイトル位置
	Vector2 m_titlePos;

    //歯車角度
	double m_gearAngle;

    //背景透明度
	int m_backAlpha;
    
};

