#pragma once
#include "Game/Entity.hpp"

class Bullet : public Entity
{
public:
	float m_damageCanInflict;
public:
	Bullet();
	~Bullet();
public:
	virtual void Update(float deltaSeconds);
	virtual void Render() const;
public:
	void SetCenterForPhysicalAndLiberalRadii();
	void UpdateBounds();
};