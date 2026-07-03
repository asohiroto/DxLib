#include "SceneBase.h"

//基底クラスでシーンのコントローラーを持たせておく
SceneBase::SceneBase(SceneController& controller):
	m_controller(controller)
{
}
