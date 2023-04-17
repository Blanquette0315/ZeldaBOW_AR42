#include "pch.h"
#include "CCollisionMgr.h"

#include "CLevelMgr.h"
#include "CLevel.h"
#include "CLayer.h"
#include "CGameObject.h"
#include "CCollider.h"
#include <PhysEngine/Export/PhysX_API.h>

CCollisionMgr::CCollisionMgr()
	: m_matrix{}
{
}

CCollisionMgr::~CCollisionMgr()
{
}

void CCollisionMgr::tick()
{
	CollisionLayerCheck(0, 0);
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();

	for (int iLayer = 0; iLayer < MAX_LAYER; ++iLayer)
	{
		const vector<CGameObject*>& vecObjects = pCurLevel->GetLayer(iLayer)->GetObjects();
		for (size_t iObjectNum = 0; iObjectNum < vecObjects.size(); ++iObjectNum)
		{
			if (nullptr == vecObjects[iObjectNum]->Collider())
				continue;

			if (!vecObjects[iObjectNum]->IsDead())
			{
				// Object has Collider Component
				PhysData* pPhysData = vecObjects[iObjectNum]->Collider()->GetPhysData();

				if (pPhysData == nullptr)
					continue;

				// EndOverlap
				if (pPhysData->Exit_Count > 0)
				{
					for (int i = 0; i < 10; i++)
					{
						PhysData* TargetData = pPhysData->TriggerExit_List[i];
						if (TargetData == nullptr) { continue; }
						if ((unsigned long long)TargetData->BOWObj == 0xdddddddddddddddd)
						{
							pPhysData->TriggerExit_List[i] = nullptr;
							pPhysData->Exit_Count--;
							continue;
						}

						CGameObject* pOther = reinterpret_cast<CGameObject*>(TargetData->BOWObj);
						if (!IsCollisionBtwLayer(vecObjects[iObjectNum]->GetLayerIdx(), pOther->GetLayerIdx()) ||
							(pOther == vecObjects[iObjectNum]))
						{
							pPhysData->TriggerExit_List[i] = nullptr;
							pPhysData->Exit_Count--;
							continue;
						}
						vecObjects[iObjectNum]->Collider()->EndOverlap(pOther);

						pPhysData->TriggerExit_List[i] = nullptr;
						pPhysData->Exit_Count--;

						// Delete EndOverlap Object by Stay_List
						for (int i = 0; i < 10; i++)
						{
							if (pPhysData->TriggerStay_List[i] == TargetData)
							{
								pPhysData->TriggerStay_List[i] = nullptr;
								pPhysData->Stay_Count--;
								break;
							}
						}

						if (pPhysData->Exit_Count <= 0)
							break;
					}
				}

				// Overlap
				if (pPhysData->Stay_Count > 0)
				{
					for (int i = 0; i < 10; i++)
					{
						PhysData* TargetData = pPhysData->TriggerStay_List[i];

						if (TargetData == nullptr) { continue; }
						if ((unsigned long long)TargetData->BOWObj == 0xdddddddddddddddd)
						{
							pPhysData->TriggerStay_List[i] = nullptr;
							pPhysData->Stay_Count--;
							continue;
						}

						CGameObject* pOther = reinterpret_cast<CGameObject*>(TargetData->BOWObj);
						// Dead check
						if (pOther->IsDead())
						{
							vecObjects[iObjectNum]->Collider()->EndOverlap(pOther);
							pPhysData->TriggerStay_List[i] = nullptr;
							pPhysData->Stay_Count--;
						}
						else
						{
							vecObjects[iObjectNum]->Collider()->Overlap(pOther);
						}
					}
				}

				// BeginOverlap
				if (pPhysData->Enter_Count > 0)
				{
					for (int i = 0; i < 10; i++)
					{
						PhysData* TargetData = pPhysData->TriggerEnter_List[i];
						if (TargetData == nullptr) { continue; }
						if ((unsigned long long)TargetData->BOWObj == 0xdddddddddddddddd)
						{
							pPhysData->TriggerEnter_List[i] = nullptr;
							pPhysData->Enter_Count--;
							continue;
						}

						CGameObject* pOther = reinterpret_cast<CGameObject*>(TargetData->BOWObj);
						if (!IsCollisionBtwLayer(vecObjects[iObjectNum]->GetLayerIdx(), pOther->GetLayerIdx()) ||
							(pOther == vecObjects[iObjectNum]))
						{
							pPhysData->TriggerEnter_List[i] = nullptr;
							pPhysData->Enter_Count--;
							continue;
						}

						// Dead check
						if (pOther->IsDead())
						{
							pPhysData->TriggerEnter_List[i] = nullptr;
							pPhysData->Enter_Count--;
						}
						else
						{
							vecObjects[iObjectNum]->Collider()->BeginOverlap(pOther);
							pPhysData->TriggerEnter_List[i] = nullptr;
							pPhysData->Enter_Count--;

							// Add Stay_List
							for (int i = 0; i < 10; i++)
							{
								if (pPhysData->TriggerStay_List[i] == nullptr)
								{
									pPhysData->TriggerStay_List[i] = TargetData;
									pPhysData->Stay_Count++;
									break;
								}
							}
						}

						if (pPhysData->Exit_Count <= 0)
							break;
					}
				}
			}
			// isDead = true
			else
			{
				// Object has Collider Component
				PhysData* pPhysData = vecObjects[iObjectNum]->Collider()->GetPhysData();

				if (pPhysData == nullptr)
					continue;

				// EndOverlap
				if (pPhysData->Exit_Count > 0)
				{
					for (int i = 0; i < 10; i++)
					{
						PhysData* TargetData = pPhysData->TriggerExit_List[i];
						if (TargetData == nullptr) { continue; }
						if ((unsigned long long)TargetData->BOWObj == 0xdddddddddddddddd)
						{
							pPhysData->TriggerExit_List[i] = nullptr;
							pPhysData->Exit_Count--;
							continue;
						}

						CGameObject* pOther = reinterpret_cast<CGameObject*>(TargetData->BOWObj);
						if (!IsCollisionBtwLayer(vecObjects[iObjectNum]->GetLayerIdx(), pOther->GetLayerIdx()) ||
							(pOther == vecObjects[iObjectNum]))
						{
							pPhysData->TriggerExit_List[i] = nullptr;
							pPhysData->Exit_Count--;
							continue;
						}

						vecObjects[iObjectNum]->Collider()->EndOverlap(pOther);

						pPhysData->TriggerExit_List[i] = nullptr;
						pPhysData->Exit_Count--;

						// Delete EndOverlap Object by Stay_List
						for (int i = 0; i < 10; i++)
						{
							if (pPhysData->TriggerStay_List[i] == TargetData)
							{
								pPhysData->TriggerStay_List[i] = nullptr;
								pPhysData->Stay_Count--;
								break;
							}
						}

						if (pPhysData->Exit_Count <= 0)
							break;
					}
				}

				// Overlap
				if (pPhysData->Stay_Count > 0)
				{
					for (int i = 0; i < 10; i++)
					{
						PhysData* TargetData = pPhysData->TriggerStay_List[i];

						if (TargetData == nullptr) { continue; }
						if ((unsigned long long)TargetData->BOWObj == 0xdddddddddddddddd)
						{
							pPhysData->TriggerStay_List[i] = nullptr;
							pPhysData->Stay_Count--;
							continue;
						}

						CGameObject* pOther = reinterpret_cast<CGameObject*>(TargetData->BOWObj);
						vecObjects[iObjectNum]->Collider()->EndOverlap(pOther);
					}
				}
			}
		}
	}
}

void CCollisionMgr::CollisionLayerCheck(int _iLeft, int _iRight)
{
	// 더 작은 숫자를 행(배열의 인덱스)으로, 더 큰 숫자를 열(비트 위치)로
	UINT iRow = 0 , iCol = 0;
	
	if (_iLeft <= _iRight)
	{
		iRow = _iLeft;
		iCol = _iRight;
	}
	else
	{
		iRow = _iRight;
		iCol = _iLeft;
	}

	m_matrix[iRow] |= (1 << iCol);
}

void CCollisionMgr::CollisionLayerRelease(int _iLeft, int _iRight)
{
	UINT iRow = 0, iCol = 0;

	if (_iLeft <= _iRight)
	{
		iRow = _iLeft;
		iCol = _iRight;
	}
	else
	{
		iRow = _iRight;
		iCol = _iLeft;
	}

	m_matrix[iRow] &= ~(1 << iCol);
}

bool CCollisionMgr::IsCollisionBtwLayer(int _iLeft, int _iRight)
{
	// 더 작은 숫자를 행(배열의 인덱스)으로, 더 큰 숫자를 열(비트 위치)로
	UINT iRow = 0 , iCol = 0;

	if (_iLeft <= _iRight)
	{
		iRow = _iLeft;
		iCol = _iRight;
	}
	else
	{
		iRow = _iRight;
		iCol = _iLeft;
	}

	return m_matrix[iRow] & (1 << iCol);
}
