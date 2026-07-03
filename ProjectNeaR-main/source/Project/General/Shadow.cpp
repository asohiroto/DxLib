#include "Shadow.h"
#include "../Game/Actor/ActorManager.h"
#include "../Game/Camera/CameraController.h"
#include <cassert>

namespace
{
	//シャドウマップの描画サイズ
	constexpr int kShadowMapWidth = 2048;
	constexpr int kShadowMapHeight = 2048;

	//シャドウマップの範囲
	constexpr float kShadowMapHorizon = 3000.0f;
	constexpr float kShadowMapVertical = 3000.0f;

	//ライトの向き
	const Vector3 kLightDir = { 0.1f,-1.0f,0.1f };
}

Shadow::Shadow():
	m_shadowMapHandle(-1)
{
	//シャドウマップ作成
	m_shadowMapHandle = MakeShadowMap(kShadowMapWidth, kShadowMapHeight);
	assert(m_shadowMapHandle != -1);
}

Shadow::~Shadow()
{
	if (m_shadowMapHandle == -1)return;
	//シャドウマップの削除
	DeleteShadowMap(m_shadowMapHandle);
}

void Shadow::DrawShadow(const std::shared_ptr<const ActorManager> pActorManager)const
{
	if (m_shadowMapHandle >= 0)
	{
		//描画に使用するシャドウマップの設定を解除
		SetUseShadowMap(0, -1);
		//ライトの向き
		SetLightDirection(kLightDir.ToDxLibVector());
		//シャドウマップが想定するライトの方向もセット
		SetShadowMapLightDirection(m_shadowMapHandle, kLightDir.ToDxLibVector());
		//シャドウマップ位置を計算(カメラ基準)
		auto cameraPos = CameraController::GetInstance().GetCameraPos();
		//最も低い位置を出す
		auto minPos = cameraPos;
		minPos.x -= kShadowMapHorizon;
		minPos.z -= kShadowMapHorizon;
		minPos.y -= kShadowMapVertical;
		//最も高い位置
		auto maxPos = cameraPos;
		maxPos.x += kShadowMapHorizon;
		maxPos.z += kShadowMapHorizon;
		maxPos.y += kShadowMapVertical;
		SetShadowMapDrawArea(m_shadowMapHandle, minPos.ToDxLibVector(), maxPos.ToDxLibVector());
		//シャドウマップへの描画の準備
		ShadowMap_DrawSetup(m_shadowMapHandle);
		//シャドウマップへのアクターの描画
		pActorManager->DrawShadow();
		//シャドウマップへの描画を終了
		ShadowMap_DrawEnd();
		//描画に使用するシャドウマップを設定
		SetUseShadowMap(0, m_shadowMapHandle);
	}

}
