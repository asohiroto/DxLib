#include "Sky.h"
#include <DxLib.h>
#include "../../../General/Model.h"
#include "../../Camera/CameraController.h"

namespace
{
	constexpr float kScale = 2.5f;
	constexpr float kRotaAngle = 0.01f;
}


Sky::Sky(std::shared_ptr<ActorData> actorData, std::weak_ptr<ActorManager> pActorManager) :
	Actor(actorData, Shape::None, pActorManager)
{
}

Sky::~Sky()
{
}

void Sky::Update()
{
	m_model->SetPos(CameraController::GetInstance().GetCameraPos().ToDxLibVector());
	m_model->SetRot(VGet(0.0f, kRotaAngle, 0.0f));
	m_model->Update();
	m_model->ApplyMat();
}

void Sky::Draw() const
{
	m_model->Draw();
}
