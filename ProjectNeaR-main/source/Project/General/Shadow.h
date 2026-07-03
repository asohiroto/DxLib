#pragma once
#include <memory>
#include "Math/MyMath.h"
class ActorManager;
class Shadow
{
public:
	Shadow();
	~Shadow();
	//影を描画する
	void DrawShadow(const std::shared_ptr<const ActorManager> pActorManager)const;

private:
	//シャドウマップハンドル
	int m_shadowMapHandle;
};

