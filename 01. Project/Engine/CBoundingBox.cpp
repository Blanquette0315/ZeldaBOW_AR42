#include "pch.h"
#include "CBoundingBox.h"

#include "CGameObject.h"
#include "CTransform.h"

float CBoundingBox::m_fDist = 3.402823466e+38;

CBoundingBox::CBoundingBox()
	: m_vCenterPos()
	, m_vCenterOffset()
	, m_vAxis{}
	, m_fExtent{}
	, m_Plane{}
{
	m_fExtent[0] = 0.5f;
	m_fExtent[1] = 0.5f;
	m_fExtent[2] = 0.5f;
}


CBoundingBox::~CBoundingBox()
{

}

//      FAR
//    6 -- 7
//   /|   /|
//  / 4 -/ 5
// 2 -- 3 /
// |    |/
// 0 -- 1
//  NEAR

CGameObject* CBoundingBox::CheckRay(tRay _ray)
{
	// picking bool should be added
	if (m_pOwner->Camera())
		return nullptr;

	CTransform* pTransform = m_pOwner->Transform();

	// Set CenterPos
	m_vCenterPos = pTransform->GetWorldPos() + m_vCenterOffset;

	//// Set Axis
	//for (int i = 0; i < (UINT)DIR::END; ++i)
	//{
	//	m_vAxis[i] = pTransform->GetRelativeDir((DIR)i);
	//}

	Vec3 vRayPos = _ray.vStart;
	Vec3 vRayDir = _ray.vDir;

	//vRayPos = vRayPos - m_vCenterPos;
	//vRayPos = XMVector3TransformCoord(vRayPos, pTransform->GetRotMatInv());
	//vRayDir = XMVector3TransformNormal(vRayDir, pTransform->GetRotMatInv());

	vRayPos = XMVector3TransformCoord(vRayPos, pTransform->GetWorldMatInv());
	vRayDir = XMVector3TransformNormal(vRayDir, pTransform->GetRotMatInv());

	// Set 8 Point
	for (int z = -1; z < 2; z+=2)
	{
		for (int y = -1; y < 2; y+=2)
		{
			for (int x = -1; x < 2; x+=2)
			{
				int index = (z + 1) / 2 * 4 + (y + 1) / 2 * 2 + (x + 1) / 2;
				m_Point[index] = Vec3( x * m_fExtent[(UINT)DIR::RIGHT], y * m_fExtent[(UINT)DIR::UP], z * m_fExtent[(UINT)DIR::FRONT] );
			}
		}
	}

	// Get Plane Normal
	m_Plane[(UINT)FACE_TYPE::FT_NEAR] = XMPlaneFromPoints(m_Point[0], m_Point[2], m_Point[3]);
	m_Plane[(UINT)FACE_TYPE::FT_FAR] = XMPlaneFromPoints(m_Point[7], m_Point[6], m_Point[4]);

	m_Plane[(UINT)FACE_TYPE::FT_UP] = XMPlaneFromPoints(m_Point[2], m_Point[6], m_Point[7]);
	m_Plane[(UINT)FACE_TYPE::FT_DOWN] = XMPlaneFromPoints(m_Point[5], m_Point[4], m_Point[0]);

	m_Plane[(UINT)FACE_TYPE::FT_LEFT] = XMPlaneFromPoints(m_Point[4], m_Point[6], m_Point[2]);
	m_Plane[(UINT)FACE_TYPE::FT_RIGHT] = XMPlaneFromPoints(m_Point[3], m_Point[7], m_Point[5]);
	
	// back direction of ray should be removed.

	for (UINT i = 0; i < (UINT)FACE_TYPE::END; ++i)
	{
		// cull - back plane or parallel plane
		if (vRayDir.Dot(m_Plane[i]) > 0)
			continue;

		// calc intersection point
		float t = -(m_Plane[i].w + m_Plane[i].x * vRayPos.x + m_Plane[i].y * vRayPos.y + m_Plane[i].z * vRayPos.z);

		if (m_Plane[i].x * vRayDir.x + m_Plane[i].y * vRayDir.y + m_Plane[i].z * vRayDir.z != 0.f)
			t /= m_Plane[i].x * vRayDir.x + m_Plane[i].y * vRayDir.y + m_Plane[i].z * vRayDir.z;
		else
			continue;
		
		Vec3 vInsecPoint = Vec3(t * vRayDir.x + vRayPos.x, t * vRayDir.y + vRayPos.y, t * vRayDir.z + vRayPos.z);

		if (vInsecPoint.x >= -m_fExtent[(UINT)DIR::RIGHT] && vInsecPoint.x <= m_fExtent[(UINT)DIR::RIGHT] &&
			vInsecPoint.y >= -m_fExtent[(UINT)DIR::UP] && vInsecPoint.y <= m_fExtent[(UINT)DIR::UP] &&
			vInsecPoint.z >= -m_fExtent[(UINT)DIR::FRONT] && vInsecPoint.z <= m_fExtent[(UINT)DIR::FRONT])
		{
			float fDist = (vInsecPoint - vRayPos).Length();
			if (fDist < m_fDist)
			{
				return m_pOwner;
			}
			else
			{
				return nullptr;
			}
			
		}
	}
	
	return nullptr;
}
