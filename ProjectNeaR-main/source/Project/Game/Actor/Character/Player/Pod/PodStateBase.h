#pragma once
#include "../../CharacterStateBase.h"
#include <memory>
class Actor;
class Pod;
class PodStateBase abstract :
    public CharacterStateBase
{
public:
    PodStateBase(std::weak_ptr<Actor>  pod);
    virtual ~PodStateBase();
protected:
	//ポッドの位置取得
    Vector3 GetPodPos(Vector3 targetPos, Vector3 dir, float upH, float right, float back);
	//ポッドの向き取得
	Vector3 GetPodDir(std::shared_ptr<Pod> owner);
};

