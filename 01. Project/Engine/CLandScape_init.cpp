#include "pch.h"
#include "CLandScape.h"

#include "CResMgr.h"
#include "CEventMgr.h"

void CLandScape::CreateMesh()
{
	// ���� �޽� ����
	Ptr<CMesh> pMesh = CResMgr::GetInst()->FindRes<CMesh>(L"LandScapeMesh");

	// �޽� �����
	// ������ �����ϴ� �޽��� ����
	if (nullptr != pMesh)
	{
		// ����
		tEvent evn = {};

		evn.eType = EVENT_TYPE::DELETE_RES;
		evn.wParam = (DWORD_PTR)RES_TYPE::MESH;
		evn.lParam = (DWORD_PTR)pMesh.Get();
		CEventMgr::GetInst()->AddEvent(evn);

		pMesh = nullptr;
	}

	vector<Vtx> vecVtx;
	vector<UINT> vecIdx;

	Vtx v;

	// ���� ��ġ
	for (UINT row = 0; row < m_iZFaceCount + 1; ++row)
	{
		for (UINT col = 0; col < m_iXFaceCount + 1; ++col)
		{
			v.vPos = Vec3((float)col, 0.f, (float)row);
			v.vUV = Vec2(col, m_iZFaceCount - row);

			v.vNormal = Vec3(0.f, 1.f, 0.f);
			v.vTangent = Vec3(1.f, 0.f, 0.f);
			v.vBinormal = Vec3(0.f, 0.f, -1.f);

			v.vColor = Vec4(1.f, 0.f, 1.f, 1.f);

			vecVtx.push_back(v);
		}
	}

	// �ε���
	for (UINT row = 0; row < m_iZFaceCount; ++row)
	{
		for (UINT col = 0; col < m_iXFaceCount; ++col)
		{
			// 0
			// | \
			// 2- 1
			vecIdx.push_back(row * (m_iXFaceCount + 1) + col + m_iXFaceCount + 1);
			vecIdx.push_back(row * (m_iXFaceCount + 1) + col + 1);
			vecIdx.push_back(row * (m_iXFaceCount + 1) + col);

			// 1- 2
			//  \ |
			//    0
			vecIdx.push_back(row * (m_iXFaceCount + 1) + col + 1);
			vecIdx.push_back(row * (m_iXFaceCount + 1) + col + m_iXFaceCount + 1);
			vecIdx.push_back(row * (m_iXFaceCount + 1) + col + m_iXFaceCount + 1 + 1);
		}
	}

	pMesh = new CMesh;
	pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	pMesh->SetKey(L"LandscapeMesh");

	tEvent evn = {};

	// �߰�
	evn = {};
	evn.eType = EVENT_TYPE::ADD_RES;
	evn.wParam = (DWORD_PTR)RES_TYPE::MESH;
	evn.lParam = (DWORD_PTR)pMesh.Get();
	CEventMgr::GetInst()->AddEvent(evn);

	SetMesh(pMesh);
}