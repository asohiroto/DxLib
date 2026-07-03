#pragma once
#include "CameraBase.h"
class TitleCamera :
    public CameraBase
{
public:
    TitleCamera();
    ~TitleCamera();
    void Init() override;
    void Update() override;
    void Draw() const override;

private:
    //Šp“x
	float m_angle;
};

