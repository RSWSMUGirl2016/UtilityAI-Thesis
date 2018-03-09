#include "Game/Bullet.hpp"
#include "Game/GameCommon.hpp"

//*******************************************************************
Bullet::Bullet()
	: m_damageCanInflict(10.f)
{
	m_isBullet = true;
}

//*******************************************************************
Bullet::~Bullet()
{
}

//*******************************************************************
void Bullet::Update(float deltaSeconds)
{
	UNUSED(deltaSeconds);
}

//*******************************************************************
void Bullet::Render() const
{
}

//*******************************************************************
void Bullet::SetCenterForPhysicalAndLiberalRadii()
{
	m_physicalDisc.center = m_position;
	m_liberalDisc.center = m_position;
}

//*******************************************************************
void Bullet::UpdateBounds()
{
	m_bounds = AABB2D(m_position - Vector2(0.15f, 0.15f), m_position + Vector2(0.15f, 0.15f));
}

