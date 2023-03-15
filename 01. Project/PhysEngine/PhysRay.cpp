#include "PhysRay.h"

PhysRayCast::PhysRayCast()
{
	m_wQueryFilterData0 = 0;
}

PhysRayCast::~PhysRayCast()
{


}

void PhysRayCast::SetStartOrigin(float& x, float& y, float& z)
{
	Origin = { x,y,z };
}

void PhysRayCast::SetStartOrigin(float&& x, float&& y, float&& z)
{
	Origin = { x,y,z };
}

void PhysRayCast::SetDirection(float& x, float& y, float& z)
{
	Direction = { x,y,z };
}

void PhysRayCast::SetDirection(float&& x, float&& y, float&& z)
{
	Direction = { x,y,z };
}

void PhysRayCast::SetMaxDistance(float& _MaxDistance)
{
	MaxDistance = _MaxDistance;
}

void PhysRayCast::SetMaxDistance(float&& _MaxDistance)
{
	MaxDistance = _MaxDistance;
}

void PhysRayCast::SetQueryFilterData0(FILTER_GROUP _eFilterGroup)
{
	m_wQueryFilterData0 |= (uint32_t)_eFilterGroup;
}

uint32_t PhysRayCast::GetQueryFilterData0()
{
	return m_wQueryFilterData0;
}