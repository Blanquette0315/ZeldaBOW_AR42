#include "RaycastManager.h"
#include "PxPhysicsAPI.h"
#include "PhysData.h"
#include "PhysRay.h"

RaycastManager::RaycastManager()
{
	m_Scene = nullptr;
}

RaycastManager::~RaycastManager()
{

}

void RaycastManager::Initialize(physx::PxScene* mScene)
{
	m_Scene = mScene;
}

bool RaycastManager::RayCast(PhysRayCast* ray)
{
	PxVec3 O = PxVec3(ray->Origin.x, ray->Origin.y, ray->Origin.z);
	PxVec3 D = PxVec3(ray->Direction.x, ray->Direction.y, ray->Direction.z);

	PxRaycastBuffer buffer;
	PxQueryFilterData QFilterData;
	QFilterData.data.word0 = ray->GetQueryFilterData0();
	bool Collision = m_Scene->raycast(O, D, ray->MaxDistance, buffer, PxHitFlags(PxHitFlag::eDEFAULT), QFilterData);

	//충돌한 카운터개수
	ray->Hit.HitCount = buffer.getNbAnyHits();
	
	ray->Hit.HitPoint.x = buffer.block.position.x;
	ray->Hit.HitPoint.y = buffer.block.position.y;
	ray->Hit.HitPoint.z = buffer.block.position.z;

	ray->Hit.HitNormal.x = buffer.block.normal.x;
	ray->Hit.HitNormal.y = buffer.block.normal.y;
	ray->Hit.HitNormal.z = buffer.block.normal.z;

	ray->Hit.FaceIndex = buffer.block.faceIndex;
	
	return Collision;
}

bool RaycastManager::RayCastMultiple(PhysRayCast* ray)
{
	PxVec3 O = PxVec3(ray->Origin.x, ray->Origin.y, ray->Origin.z);
	PxVec3 D = PxVec3(ray->Direction.x, ray->Direction.y, ray->Direction.z);

	const PxU32 bufferSize = 100;
	PxRaycastHit hitbuffer[bufferSize];
	PxRaycastBuffer buf(hitbuffer, bufferSize);
	

	bool Collision = m_Scene->raycast(O, D, ray->MaxDistance, buf);
	
	for (PxU32 i = 0; i < buf.nbTouches; i++) 
	{
		buf.getTouch(i).position;
	}

	return Collision;
}
