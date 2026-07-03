#pragma once
#include "../../../../General/Collision/Collidable.h"
class AvoidColl :
    public Collidable
{
public:
	AvoidColl(float radius, unsigned int playerID);
	~AvoidColl();
	void Update(Vector3 nextPos,bool isEnableColl);
	void Draw()const;
	void OnCollide(const std::shared_ptr<Collidable> other)override;
	void Complete() override {};

	//ìñÇΩÇ¡ÇΩÇ©
	bool IsHit()const { return m_isHit; };
private:
	//ìñÇΩÇ¡ÇΩÇ©
	bool m_isHit;
	//ÉvÉåÉCÉÑÅ[ÇÃID
	unsigned int m_playerID;
};

