#include "pch.h"
#include "CPhysMgr.h"

#include <PhysEngine/Export/PhysX_API.h>

#include "CKeyMgr.h"
#include "CTimeMgr.h"

CPhysMgr::CPhysMgr()
	: m_pPhys(nullptr)
	, m_fFrame(0.f)//1.0f / 60.f)
	, m_pData(nullptr)
{
}

CPhysMgr::~CPhysMgr()
{
}

void CPhysMgr::init()
{
	PhysX_Initialize(1, true);

	//PhysData* pData = new PhysData;
	//pData->SetWorldPosition(0.f, 0.f, 0.f);
	//pData->mCollider->CreateBox(1.f, 0.01f, 1.f);
	//pData->mMeterial->MT_Restitution = 0.f;
	//pData->isKinematic = true;
	//PhysX_Create_Actor(pData);

	//pData = new PhysData;
	//pData->mCollider->CreateBox(0.1f, 1.f, 1.f);
	//pData->SetWorldPosition(1.f, 0.02f, 0.f);
	//pData->mMeterial->MT_Restitution = 0.f;
	//pData->isKinematic = true;
	//PhysX_Create_Actor(pData);

	////pData = new PhysData;
	////pData->mCollider->CreateBox(0.1f, 0.1f, 0.1f);
	////pData->SetWorldPosition(0.03f, 1.f, 0.f);
	////pData->mMeterial->MT_Restitution = 0.f;
	////pData->isDinamic = true;

	////PhysX_Create_Actor(pData);

	//m_pData = new PhysData;
	//m_pData->SetWorldPosition(0.f, 0.5f, 0.f);
	//m_pData->mCollider->SetSphereCollider(0.1f);
	//m_pData->isDinamic = true;
	//m_pData->mMeterial->MT_Restitution = 0.f;
	//m_pData->SetLockAxis_Position(false, false, false);
	//m_pData->SetLockAxis_Rotation(true, true, true);
	////m_pData->SetTrigger(true);
	//PhysX_Create_Actor(m_pData);

	PhysData* pData = new PhysData;
	pData->SetWorldPosition(0.f, 0.f, 0.f);
	pData->mCollider->CreateBox(100.f, 1.f, 100.f);
	pData->mMeterial->MT_Restitution = 0.f;
	pData->isKinematic = true;
	PhysX_Create_Actor(pData);

	//pData = new PhysData;
	//pData->mCollider->CreateBox(0.1f, 0.1f, 0.1f);
	//pData->SetWorldPosition(0.03f, 1.f, 0.f);
	//pData->mMeterial->MT_Restitution = 0.f;
	//pData->isDinamic = true;

	//PhysX_Create_Actor(pData);

	m_pData = new PhysData;
	m_pData->SetWorldPosition(0.f, 50.f, 0.f);
	m_pData->mCollider->SetSphereCollider(10.f);
	m_pData->isDinamic = true;
	m_pData->mMeterial->MT_Restitution = 0.f;
	m_pData->SetLockAxis_Position(false, false, false);
	m_pData->SetLockAxis_Rotation(true, true, true);
	//m_pData->SetTrigger(true);
	PhysX_Create_Actor(m_pData);
}

void CPhysMgr::tick()
{
	//m_fFrame = DT;
	PhysX_Update(FDT);
	m_fFrame += FDT;
	float dtd = m_pData->mMeterial->MT_Restitution;
	PhysX_Update_Actor(m_pData);

	//if(m_pData->mCollider->GetTrigger)
	if (KEY_PRESSED(KEY::SPACE))
	{
		m_pData->SetVelocity(0.2f, 0.f, 0.f);
	}

}